
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
    else if (name == "80store")
        _80store = getOEInt(value);
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
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
    }
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "keyboard")
        keyboard = ref;
    else if (name == "memoryBus")
        memoryBus = ref;
   else if (name == "video")
        video = ref;
	else
		return false;
	
	return true;
}

bool AppleIIEMMU::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    OECheckComponent(video);
    OECheckComponent(memoryBus);
    OECheckComponent(bankSwitcher);

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

    updateBankSwitcher();
    updateBankOffset();

    return true;
}

void AppleIIEMMU::notify(OEComponent *sender, int notification, void *data)
{
    // TODO(zellyn): handle resets too. This is just copied from AppleLanguageCard.cpp
    ControlBusPowerState powerState = *((ControlBusPowerState *)data);
    
    if (powerState == CONTROLBUS_POWERSTATE_OFF)
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
            case 0xC01C: val = getVideoBool("page2"); break;
            case 0xC01D: val = getVideoBool("hires"); break;
            case 0xC01E: val = getVideoBool("altchrset"); break;
            case 0xC01F: val = getVideoBool("80col"); break;
        }
        if (val) {
            return 0x80 | kbd;
        } else {
            return kbd;
        }
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
            case 0xC000: if (_80store) { _80store = false; updateAuxmem(); }; break;
            case 0xC001: if (!_80store) { _80store = true; updateAuxmem(); }; break;
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
    
}

void AppleIIEMMU::updateCxxxRom() {
    
}
