
/**
 * libemulation
 * Apple IIe MMU softswitch handler.
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com), but with chunks
 * copied from AppleLanguageCard.cpp
 * Released under the GPL
 *
 * Controls MMU (and IOU) softswitch setting/resetting/reading.
 */

#include "AppleIIEMMU.h"

#include "AppleIIInterface.h"
#include "ControlBusInterface.h"

AppleIIEMMU::AppleIIEMMU()
{
    bankSwitcher = NULL;
    controlBus = NULL;
	floatingBus = NULL;
    memoryBus = NULL;
    ramMapper = NULL;
    romC0DF = NULL;
    video = NULL;
    
    bank1 = false;
    hramRead = false;
    preWrite = false;
    hramWrite = false;

    ramrd = false;
    ramwrt = false;
    _80store = false;
    intcxrom = false;
    altzp = false;
    slotc3rom = false;
    intc8rom = false;
    
    hires = false;
    page2 = false;
}

bool AppleIIEMMU::setValue(string name, string value)
{
    if (name == "bank1")
        bank1 = getOEInt(value);
    else if (name == "hramRead")
        hramRead = getOEInt(value);
    else if (name == "preWrite")
        preWrite = getOEInt(value);
    else if (name == "hramWrite")
        hramWrite = getOEInt(value);
    else if (name == "ramrd")
        ramrd = getOEInt(value);
    else if (name == "ramwrt")
        ramwrt = getOEInt(value);
    else if (name == "80store") {
        _80store = getOEInt(value);
    }
    else if (name == "intcxrom")
        intcxrom = getOEInt(value);
    else if (name == "altzp")
        altzp = getOEInt(value);
    else if (name == "slotc3rom")
        slotc3rom = getOEInt(value);
    else if (name == "intc8rom")
        intc8rom = getOEInt(value);
    else
        return false;
    
    return true;
}

bool AppleIIEMMU::getValue(string name, string& value)
{
    if (name == "bank1")
        value = getString(bank1);
    else if (name == "hramRead")
        value = getString(hramRead);
    else if (name == "preWrite")
        value = getString(preWrite);
    else if (name == "hramWrite")
        value = getString(hramWrite);
    else if (name == "ramrd")
        value = getString(ramrd);
    else if (name == "ramwrt")
        value = getString(ramwrt);
    else if (name == "80store")
        value = getString(_80store);
    else if (name == "intcxrom")
        value = getString(intcxrom);
    else if (name == "altzp")
        value = getString(altzp);
    else if (name == "slotc3rom")
        value = getString(slotc3rom);
    else if (name == "intc8rom")
        value = getString(intc8rom);
    else
        return false;
    
    return true;
}

bool AppleIIEMMU::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
        {
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        }
        controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        }
    }
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "keyboard")
        keyboard = ref;
    else if (name == "memoryBus")
        memoryBus = ref;
    else if (name == "ramMapper")
        ramMapper = ref;
    else if (name == "romC0DF")
        romC0DF = ref;
    else if (name == "video") {
        if (video) {
            video->removeObserver(this, APPLEII_HIRES_DID_CHANGE);
            video->removeObserver(this, APPLEII_PAGE2_DID_CHANGE);
        }
        video = ref;
        if (video) {
            video->addObserver(this, APPLEII_HIRES_DID_CHANGE);
            video->addObserver(this, APPLEII_PAGE2_DID_CHANGE);
        }
    }
	else
		return false;
	
	return true;
}

bool AppleIIEMMU::init()
{
    OECheckComponent(bankSwitcher);
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    OECheckComponent(memoryBus);
    OECheckComponent(ramMapper);
    OECheckComponent(romC0DF);
    OECheckComponent(video);

    string videoModel;
    if (!video->getValue("model", videoModel) || videoModel != "IIe") {
        logMessage("video component 'model' must be IIe; got '" + videoModel + "'");
        return false;
    }

    hramMap.component = bankSwitcher;
    hramMap.startAddress = 0xd000;
    hramMap.endAddress = 0xffff;
    hramMap.read = false;
    hramMap.write = false;    

    // Initialize maps used to send Cxxx ROM reads/writes to the MMU.
    for (int i=0; i < 4; i++) {
        cxxxMaps[i].component = this;
        cxxxMaps[i].read = false;
        cxxxMaps[i].write = false;
    }
    cxxxMaps[0].startAddress = 0xC100; cxxxMaps[0].endAddress = 0xC2ff;
    cxxxMaps[1].startAddress = 0xC300; cxxxMaps[1].endAddress = 0xC3ff;
    cxxxMaps[2].startAddress = 0xC400; cxxxMaps[2].endAddress = 0xC7ff;
    cxxxMaps[3].startAddress = 0xC800; cxxxMaps[3].endAddress = 0xCfff;

    updateBankSwitcher();
    updateBankOffset();

    updateAuxmem();
    updateCxxxRom();

    return true;
}

void AppleIIEMMU::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus) {
        
        bool reset = (notification==CONTROLBUS_RESET_DID_ASSERT);
        if (notification==CONTROLBUS_POWERSTATE_DID_CHANGE) {
            ControlBusPowerState powerState = *((ControlBusPowerState *)data);
            reset = reset || (powerState == CONTROLBUS_POWERSTATE_OFF);
        }
            
        if (reset)
        {
            bank1 = false;
            hramRead = false;
            preWrite = false;
            hramWrite = false;

            ramrd = false;
            ramwrt = false;
            _80store = false;
            intcxrom = false;
            altzp = false;
            slotc3rom = false;
            intc8rom = false;

            updateBankSwitcher();
            updateBankOffset();
            updateAuxmem();
            updateCxxxRom();
        }
    }
    
    if (sender == video) {
        bool value = *((bool *)data);
        if (notification==APPLEII_HIRES_DID_CHANGE) {
            hires = value;
            updateAuxmem();
        }
        else if (notification==APPLEII_PAGE2_DID_CHANGE) {
            page2 = value;
            updateAuxmem();
        }
    }
}

OEChar AppleIIEMMU::read(OEAddress address)
{
    // High RAM.
    if ((address&0xFFF0)==0xC080) {
        if (address & 0x1)
        {
            if (preWrite)
                hramWrite = true;
        }
        else
            hramWrite = false;
        preWrite = address & 0x1;
        
        hramRead = !(((address >> 1) ^ address) & 1);
        setBank1(address & 0x8);
        updateBankSwitcher();
    }
    
    // Read softswitches
    else if ((address>=0xC011) && (address<=0xC01F)) {
        OEChar kbd = keyboard->read(0) & 0x7f;
        bool val = 0;
        switch (address) {
            case 0xC011: val = !bank1; break;
            case 0xC012: val = hramRead; break;
            case 0xC013: val = ramrd; break;
            case 0xC014: val = ramwrt; break;
            case 0xC015: val = intcxrom; break;
            case 0xC016: val = altzp; break;
            case 0xC017: val = slotc3rom; break;
            case 0xC018: val = _80store; break;
            case 0xC019:
                bool vbl;
                video->postMessage(this, APPLEII_IS_VBL, &vbl);
                val = !vbl; break;
            case 0xC01A: val = getVideoBool("text"); break;
            case 0xC01B: val = getVideoBool("mixed"); break;
            case 0xC01C: val = page2; break;
            case 0xC01D: val = hires; break;
            case 0xC01E: val = getVideoBool("altchrset"); break;
            case 0xC01F: val = getVideoBool("80col"); break;
        }
        if (val) {
            return 0x80 | kbd;
        } else {
            return kbd;
        }
    }

    else if ((address>=0xc100)&&(address<=0xcfff)) {
        if ((address>=0xc300)&&(address<=0xc3ff)) {
            if (!slotc3rom && !intc8rom) {
                intc8rom = true;
                updateCxxxRom();
            }
        }
        else if (address==0xcfff) {
            if (intc8rom) {
                intc8rom = false;
                updateCxxxRom();
                if (!intcxrom) {
                    memoryBus->read(address);
                }
            }
        }
        return romC0DF->read(address);
    }

    return floatingBus->read(address);
}

bool AppleIIEMMU::getVideoBool(string property) {
    string value;
    if (!video->getValue(property, value)) {
        logMessage("unable to retrieve video property '" + property + "'");
        return false;
    };
    return getOEInt(value);
}

void AppleIIEMMU::write(OEAddress address, OEChar value)
{
    // High RAM.
    if ((address&0xFFF0)==0xC080) {
        if (!(address & 0x1))
            hramWrite = false;

        preWrite = false;
        hramRead = !(((address >> 1) ^ address) & 1);
        setBank1(address & 0x8);
        updateBankSwitcher();
    }

    // Softswitches
    else if ((address&0xFFF0)==0xC000) {
        switch(address) {
            case 0xC000: if (_80store) {
                _80store = false;
                updateAuxmem();
                this->postNotification(this, APPLEII_80STORE_DID_CHANGE, &_80store);
            }; break;
            case 0xC001: if (!_80store) {
                _80store = true;
                updateAuxmem();
                this->postNotification(this, APPLEII_80STORE_DID_CHANGE, &_80store);
            }; break;
            case 0xC002: if (ramrd) { ramrd = false; updateAuxmem(); }; break;
            case 0xC003: if (!ramrd) { ramrd = true; updateAuxmem(); }; break;
            case 0xC004: if (ramwrt) { ramwrt = false; updateAuxmem(); }; break;
            case 0xC005: if (!ramwrt) { ramwrt = true; updateAuxmem(); }; break;
            case 0xC006: if (intcxrom) { intcxrom = false; updateCxxxRom(); }; break;
            case 0xC007: if (!intcxrom) { intcxrom = true; updateCxxxRom(); }; break;
            case 0xC008: if (altzp) { altzp = false; updateAuxmem(); }; break;
            case 0xC009: if (!altzp) { altzp = true; updateAuxmem(); }; break;
            case 0xC00A: if (slotc3rom) { slotc3rom = false; updateCxxxRom(); }; break;
            case 0xC00B: if (!slotc3rom) { slotc3rom = true; updateCxxxRom(); }; break;
        }
    }
    else if ((address>=0xc300)&&(address<=0xc3ff)) {
        if (!slotc3rom && !intc8rom) {
            intc8rom = true;
            updateCxxxRom();
        }
    }
    else if (address==0xcfff) {
        if (intc8rom) {
            intc8rom = false;
            updateCxxxRom();
        }
    }
}

void AppleIIEMMU::setBank1(bool value)
{
    if (bank1 == value)
        return;
    
    bank1 = value;
    
    updateBankOffset();
}

void AppleIIEMMU::updateBankOffset()
{
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x1000;
    offsetMap.endAddress = 0x1fff;
    offsetMap.offset = bank1 ? -0x1000 : 0x0000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}

void AppleIIEMMU::updateBankSwitcher()
{
    if ((hramMap.read == hramRead) &&
        (hramMap.write == hramWrite))
        return;
    
    if (hramMap.read || hramMap.write)
        memoryBus->postMessage(this, APPLEII_UNMAP_INTERNAL, &hramMap);
    
    hramMap.read = hramRead;
    hramMap.write = hramWrite;
    
    if (hramMap.read || hramMap.write)
        memoryBus->postMessage(this, APPLEII_MAP_INTERNAL, &hramMap);
}

void AppleIIEMMU::updateAuxmem() {
    string sel;

    sel = altzp ? "Aux00_01,AuxC0_FF" : "Main00_01,MainC0_FF";
    
    if (ramrd) {
        sel += ",AuxR02_03,AuxR08_1F,AuxR40_BF";
        if (!_80store) sel += ",AuxR04_07";
        if (!_80store||!hires) sel += ",AuxR20_3F";
    } else {
        sel += ",MainR02_03,MainR08_1F,MainR40_BF";
        if (!_80store) sel += ",MainR04_07";
        if (!_80store||!hires) sel += ",MainR20_3F";
    }
    if (ramwrt) {
        sel += ",AuxW02_03,AuxW08_1F,AuxW40_BF";
        if (!_80store) sel += ",AuxW04_07";
        if (!_80store||!hires) sel += ",AuxW20_3F";
    } else {
        sel += ",MainW02_03,MainW08_1F,MainW40_BF";
        if (!_80store) sel += ",MainW04_07";
        if (!_80store||!hires) sel += ",MainW20_3F";
    }

    if (_80store) {
        if (page2) {
            sel += ",AuxR04_07,AuxW04_07";
            if (hires) {
                sel += ",AuxR20_3F,AuxW20_3F";
            }
        }
        else {
            sel += ",MainR04_07,MainW04_07";
            if (hires) {
                sel += ",MainR20_3F,MainW20_3F";
            }
        }
    }
    ramMapper->postMessage(this, ADDRESSMAPPER_SELECT, &sel);
}

void AppleIIEMMU::updateCxxxRom() {
    for (int i=0; i < 4; i++) {
        MemoryMap *map = &cxxxMaps[i];
        if (map->read||map->write)
            memoryBus->postMessage(this, APPLEII_UNMAP_CXXX, map);
    }

    cxxxMaps[0].read = intcxrom;
    cxxxMaps[1].read = intcxrom || !slotc3rom;
    cxxxMaps[1].write = intcxrom || !slotc3rom;
    cxxxMaps[2].read = intcxrom;
    cxxxMaps[3].read = intcxrom || intc8rom;
    cxxxMaps[3].write = intcxrom || intc8rom;

    for (int i=0; i < 4; i++) {
        MemoryMap *map = &cxxxMaps[i];
        if (map->read||map->write)
            memoryBus->postMessage(this, APPLEII_MAP_CXXX, map);
    }
}
