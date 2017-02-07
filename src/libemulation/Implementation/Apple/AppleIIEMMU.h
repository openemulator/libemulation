
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
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);

private:
    OEComponent *bankSwitcher;
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *keyboard;
    OEComponent *memoryBus;
    OEComponent *ramMapper;
    OEComponent *romC0DF;
    OEComponent *video;
    
    bool bank1;
    bool hramRead;
    bool preWrite;
    bool hramWrite;
    MemoryMap hramMap;
    
    bool ramrd;
    bool ramwrt;
    bool _80store;
    bool intcxrom;
    bool altzp;
    bool slotc3rom;
    bool intc8rom;
    
    // Copies of the video's values
    bool hires;
    bool page2;
    
    void setBank1(bool value);
    void updateBankOffset();
    void updateBankSwitcher();

    bool getVideoBool(string property);
    void updateAuxmem();
    
    MemoryMap cxxxMaps[4];
    void updateCxxxRom();
};
