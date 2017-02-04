
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

#include "ControlBusInterface.h"

AppleIIEMMU::AppleIIEMMU()
{
    controlBus = NULL;
	floatingBus = NULL;
    video = NULL;
    
    memoryBus = NULL;
    bankSwitcher = NULL;
    
    bank1 = false;
    hramRead = false;
    preWrite = false;
    hramWrite = false;
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
    else if (name == "memoryBus")
        memoryBus = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
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
    return floatingBus->read(address);
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
        memoryBus->postMessage(this, ADDRESSDECODER_UNMAP, &hramMap);
    
    hramMap.read = hramRead;
    hramMap.write = hramWrite;
    
    if (hramMap.read || hramMap.write)
        memoryBus->postMessage(this, ADDRESSDECODER_MAP, &hramMap);
}
