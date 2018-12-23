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

#define WOZ_SIGNATURE           0x574f5a00
#define WOZ_INTEGRITYCHECK      0x0a0d0aff

#define WOZ_INFO_START          20
#define WOZ_INFO_SIZE           60

#define WOZ_TRACKMAP_START      88
#define WOZ_TRACKMAP_SIZE       160

#define WOZ1_TRACKS_START       256
#define WOZ1_TRACKS_SIZE        6656

#define WOZ1_TRACK_INFO_BYTES   6646
#define WOZ1_TRACK_INFO_BITS    6648

#define WOZ2_TRK_START          256
#define WOZ2_TRK_SIZE           8

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
    DIInt signature = getDIIntBE(&header[0]);
    if((signature & 0xffffff00) != WOZ_SIGNATURE)
        return false;
    wozVersion = signature & 0x000000ff;
    // validate the integrity check
    if(getDIIntLE(&header[4]) != WOZ_INTEGRITYCHECK)
        return false;
    
    // grab stuff from the INFO chunk
    DIChar info[WOZ_INFO_SIZE];
    if(!backingStore->read(WOZ_INFO_START, info, WOZ_INFO_SIZE))
        return false;
    DIChar infoVersion = info[0];
    if(info[1] != 1)    // be sure that we are a 5.25 disk
        return false;
    writeProtected = info[2] == 1;
    if(infoVersion >= 2) {
        optimalBitTiming = info[39];
        largestTrack = getDIShortLE(&info[44]) * 512;
    } else {
        optimalBitTiming = 32;
        largestTrack = WOZ1_TRACKS_SIZE;
    }
    
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
    
    DIChar trackData[largestTrack];
    DIInt trackStart;
    DIInt trackBitCount;
    
    if(wozVersion >= 2) {
        // WOZ 2.0
        DIChar trk[WOZ2_TRK_SIZE];
        DIInt trkStart = WOZ2_TRK_START + (realTrack * WOZ2_TRK_SIZE);
        if (!backingStore->read(trkStart, trk, WOZ2_TRK_SIZE))
            return false;
        trackStart = ((DIInt)getDIShortLE(&trk[0])) << 9;
        DIInt trackSize = ((DIInt)getDIShortLE(&trk[2])) << 9;
        trackBitCount = getDIIntLE(&trk[4]);
        if (!backingStore->read(trackStart, trackData, trackSize))
            return false;
    } else {
        // WOZ 1.0
        trackStart = WOZ1_TRACKS_START + (realTrack * WOZ1_TRACKS_SIZE);
        if (!backingStore->read(trackStart, trackData, WOZ1_TRACKS_SIZE))
            return false;
        trackBitCount = getDIShortLE(&trackData[WOZ1_TRACK_INFO_BITS]);
    }
    
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
