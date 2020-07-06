/**
 * libdiskimage
 * Disk Image WOZ
 * (C) 2018 by John K. Morris (jmorris@evolutioninteractive.com)
 * Released under the GPL
 *
 * Accesses WOZ disk images
 */

#include "DIBackingStore.h"
#include "DIDiskStorage.h"

class DIWozDiskStorage : public DIDiskStorage
{
public:
    DIWozDiskStorage();
    ~DIWozDiskStorage();
    
    bool open(DIBackingStore *file);
    void close();
    
    bool isWriteEnabled();
    DIDiskType getDiskType();
    DIInt getTracksPerInch();
    string getFormatLabel();
    DILong getOptimalBitTiming();
    
    bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    bool writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);

private:
    
    DIChar wozVersion;
    bool writeProtected;
    DIChar optimalBitTiming;
    DIInt largestTrack;
    bool updateCRC32;

    DIBackingStore *backingStore;
    
    DIData trackMap;

    DIInt crc32(DIInt crc, const DIChar *buf, DILong size);
};
