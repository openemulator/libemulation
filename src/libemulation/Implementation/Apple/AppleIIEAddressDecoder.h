
/**
 * libemulation
 * Apple II Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II Address Decoder
 */

#include "AppleIIAddressDecoder.h"

class AppleIIEAddressDecoder : public AppleIIAddressDecoder
{
public:
	AppleIIEAddressDecoder();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    MemoryMaps cxxxMemoryMaps;
    
    void updateReadWriteMap(OEAddress startAddress, OEAddress endAddress);
};
