
/**
 * libemulation
 * Apple II Address Decoder
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls an Apple II Address Decoder
 */

#include "AppleIIEAddressDecoder.h"

#include "AppleIIInterface.h"

AppleIIEAddressDecoder::AppleIIEAddressDecoder() : AppleIIAddressDecoder()
{
}

bool AppleIIEAddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case ADDRESSDECODER_MAP:
            return addMemoryMap(externalMemoryMaps, (MemoryMap *) data);

        case ADDRESSDECODER_UNMAP:
            return removeMemoryMap(externalMemoryMaps, (MemoryMap *) data);

        case APPLEII_MAP_SLOT:
            return addMemoryMap(ioMemoryMaps, (MemoryMap *) data);

        case APPLEII_UNMAP_SLOT:
            return removeMemoryMap(ioMemoryMaps, (MemoryMap *) data);

        case APPLEII_MAP_CXXX:
            return addMemoryMap(cxxxMemoryMaps, (MemoryMap *) data);
            
        case APPLEII_UNMAP_CXXX:
            return removeMemoryMap(cxxxMemoryMaps, (MemoryMap *) data);

        case APPLEII_MAP_INTERNAL:
            return addMemoryMap(internalMemoryMaps, (MemoryMap *) data);
            
        case APPLEII_UNMAP_INTERNAL:
            return removeMemoryMap(internalMemoryMaps, (MemoryMap *) data);
    }

    return false;
}

void AppleIIEAddressDecoder::updateReadWriteMap(OEAddress startAddress, OEAddress endAddress)
{
    AddressDecoder::updateReadWriteMap(internalMemoryMaps, startAddress, endAddress);
    AddressDecoder::updateReadWriteMap(ioMemoryMaps, startAddress, endAddress);
    AddressDecoder::updateReadWriteMap(cxxxMemoryMaps, startAddress, endAddress);
    AddressDecoder::updateReadWriteMap(externalMemoryMaps, startAddress, endAddress);
}

