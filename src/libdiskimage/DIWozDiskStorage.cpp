/**
 * libdiskimage
 * Disk Image WOZ
 * (C) 2018 by John K. Morris (jmorris@evolutioninteractive.com)
 * Released under the GPL
 *
 * Accesses WOZ disk images
 */

#include "DIWozDiskStorage.h"

#define WOZ_HEADER_START        0
#define WOZ_HEADER_SIZE         8

#define WOZ_SIGNATURE           0x315A4F57
#define WOZ_INTEGRITYCHECK      0x0a0d0aff

#define WOZ_TRACKMAP_START      88
#define WOZ_TRACKMAP_SIZE       160

#define WOZ_TRACKS_START        256
#define WOZ_TRACKS_SIZE         6656

#define WOZ_TRACK_INFO_BYTES    6646
#define WOZ_TRACK_INFO_BITS     6648

#define WOZ_UNFORMATTED_SIZE    6250

DIWozDiskStorage::DIWozDiskStorage()
{
    close();
}

DIWozDiskStorage::~DIWozDiskStorage()
{
    close();
}

bool DIWozDiskStorage::open(DIBackingStore *backingStore)
{
    DIChar header[WOZ_HEADER_SIZE];
    
    // load in the header
    if(!backingStore->read(WOZ_HEADER_START, header, WOZ_HEADER_SIZE))
        return false;
    
    // validate the signature
    if(getDIIntLE(&header[0]) != WOZ_SIGNATURE)
        return false;
    // validate the integrity check
    if(getDIIntLE(&header[4]) != WOZ_INTEGRITYCHECK)
        return false;
    
    // load up the TMAP chunk
    trackMap.resize(WOZ_TRACKMAP_SIZE);
    if(!backingStore->read(WOZ_TRACKMAP_START, &trackMap.front(), WOZ_TRACKMAP_SIZE))
        return false;
        
    this->backingStore = backingStore;
    
    return true;
}

void DIWozDiskStorage::close()
{
    backingStore = NULL;
}

bool DIWozDiskStorage::isWriteEnabled()
{
    return false;
}

DIDiskType DIWozDiskStorage::getDiskType()
{
    return DI_525_INCH;
}

DIInt DIWozDiskStorage::getTracksPerInch()
{
    return 192;
}

string DIWozDiskStorage::getFormatLabel()
{
    return "WOZ Disk Image (read-only)";
}

bool DIWozDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    if (trackIndex >= WOZ_TRACKMAP_SIZE)
        return false;
    
    // remap the track based on the track map
    DIInt realTrack = trackMap[trackIndex];
    if (realTrack == 0xff) {
        // we have an unformatted track, so we are going to give a track stuffed with zeros so that the fake bits fire
        track.format = DI_APPLE_NIB;
        track.data.resize(WOZ_UNFORMATTED_SIZE);
        memset(&track.data.front(), 0, WOZ_UNFORMATTED_SIZE);
        return true;
    }
    // calc the track start
    DIInt trackStart = WOZ_TRACKS_START + (realTrack * WOZ_TRACKS_SIZE);
    // grab the track info
    DIChar trackData[WOZ_TRACKS_SIZE];
    if (!backingStore->read(trackStart, trackData, WOZ_TRACKS_SIZE))
        return false;
    DIInt trackSize = getDIShortLE(&trackData[WOZ_TRACK_INFO_BYTES]);
    DIInt trackBitCount = getDIShortLE(&trackData[WOZ_TRACK_INFO_BITS]);
    
    // setup the output track
    track.data.resize(trackBitCount);
    track.format = DI_BITSTREAM_250000BPS;
    
    // convert the bitstream
    DIChar codeTable[] = {0x00, 0xff};
    for (DIInt i = 0; i < trackBitCount; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        track.data[i] = codeTable[(trackData[byteIndex] >> bitIndex) & 0x1];
    }

    return true;
}

bool DIWozDiskStorage::writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    return false;
}
