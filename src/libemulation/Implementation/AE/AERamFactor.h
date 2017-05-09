
/**
 * libemulation
 * Applied Engineering RamFactor Card
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls an Applied Engineering RamFactor Card
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class AERamFactor : public OEComponent
{
public:
	AERamFactor();
	
	bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    void dispose();
	
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
    void enableRamFactor(bool value);
    void updateRamFactor(bool value);
    
    void setFirmwareBank(OEChar value);
    void updateBankOffset();
    OEAddress incAddr();
    
private:
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *bankSwitcher;
    OEComponent *ram;

    OEAddress address;
    OEChar bank;
    bool hiOrF;
};
