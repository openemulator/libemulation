
/**
 * libemulation
 * Apple IIe MMU softswitch handler.
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls MMU (and IOU) softswitch setting/resetting/reading.
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class AppleIIEMMU : public OEComponent
{
public:
    AppleIIEMMU();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *video;
};
