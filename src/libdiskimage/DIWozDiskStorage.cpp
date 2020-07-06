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

#define WOZ_CRC32_START         8
#define WOZ_CRC32_SIZE          4

#define WOZ_CHUNK_START         12

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

static DIInt crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

DIWozDiskStorage::DIWozDiskStorage()
{
    updateCRC32 = false;
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
    wozVersion = (signature & 0x000000ff) - 0x30;   // converting from ASCII to binary
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
    updateCRC32 = false;

    return true;
}

void DIWozDiskStorage::close()
{
    if(updateCRC32) {
        DILong wozSize = backingStore->getSize() - WOZ_CHUNK_START;
        DIChar wozBuf[wozSize];
        DIInt crc = 0;
        DIChar crcBuf[WOZ_CRC32_SIZE];

        backingStore->read(WOZ_CHUNK_START, wozBuf, wozSize);

        crc = crc32(crc, wozBuf, wozSize);
        crcBuf[3] = (crc >> 24) & 0xff;
        crcBuf[2] = (crc >> 16) & 0xff;
        crcBuf[1] = (crc >> 8) & 0xff;
        crcBuf[0] = crc & 0xff;

        backingStore->write(WOZ_CRC32_START, crcBuf, WOZ_CRC32_SIZE);

        updateCRC32 = false;
    }

    backingStore = NULL;
}

bool DIWozDiskStorage::isWriteEnabled()
{
    return !writeProtected;
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
    std::string label = "WOZ";

    if(wozVersion>=2) label += "2";
    else label += "1";
    label += " Disk Image";

    if(writeProtected) label += " (read-only)";

    return label;
}

DILong DIWozDiskStorage::getOptimalBitTiming() {
    return optimalBitTiming;
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
    if (trackIndex >= WOZ_TRACKMAP_SIZE)
        return false;

    // remap the track based on the track map
    DIInt realTrack = trackMap[trackIndex];
    if (realTrack == 0xff)
        return true;

    DIInt trackStart;
    DIInt trackBitCount;
    DIInt trackSize;
    DIChar trackData[largestTrack];
    memset(trackData, 0, largestTrack);

    if(wozVersion >= 2) {
        // WOZ 2.0
        DIChar trk[WOZ2_TRK_SIZE];
        DIInt trkStart = WOZ2_TRK_START + (realTrack * WOZ2_TRK_SIZE);
        if (!backingStore->read(trkStart, trk, WOZ2_TRK_SIZE))
            return false;
        trackStart = ((DIInt)getDIShortLE(&trk[0])) << 9;
        trackSize = ((DIInt)getDIShortLE(&trk[2])) << 9;
        trackBitCount = getDIIntLE(&trk[4]);
    } else {
        // WOZ 1.0
        trackStart = WOZ1_TRACKS_START + (realTrack * WOZ1_TRACKS_SIZE);
        trackSize = WOZ1_TRACKS_SIZE;
        if (!backingStore->read(trackStart+WOZ1_TRACK_INFO_BYTES, &trackData[WOZ1_TRACK_INFO_BYTES], WOZ1_TRACKS_SIZE-WOZ1_TRACK_INFO_BYTES))
            return false;
        trackBitCount = getDIShortLE(&trackData[WOZ1_TRACK_INFO_BITS]);
    }

    // compare and copy the bitstream
    for (DIInt i = 0; i < trackBitCount; i++) {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);

        if(track.data[i])
            trackData[byteIndex] |= (0x1 << bitIndex);
    }

    if (!backingStore->write(trackStart, trackData, trackSize))
        return false;

    updateCRC32 = true;

    return true;
}

DIInt DIWozDiskStorage::crc32(DIInt crc, const DIChar *buf, DILong size)
{
    const DIChar *p;
    p = buf;
    crc = crc ^ ~0U;
    while (size--)
    crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return crc ^ ~0U;
}

