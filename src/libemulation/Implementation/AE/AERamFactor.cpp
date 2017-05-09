/**
 * libemulation
 * Applied Engineering RamFactor Card
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls an Applied Engineering RamFactor Card
 */

#include "AERamFactor.h"

#include "ControlBusInterface.h"
#include "MemoryInterface.h"
#include "AppleIIInterface.h"

AERamFactor::AERamFactor()
{
    controlBus = NULL;
    floatingBus = NULL;
    bankSwitcher = NULL;
    ram = NULL;

    address = 0;
    bank = 0;
    hiOrF = false;
}

bool AERamFactor::setValue(string name, string value)
{
    if (name == "address")
        address = getOELong(value);
    else if (name == "bank")
        bank = getOEInt(value);
    else
        return false;
    
    return true;
}

bool AERamFactor::getValue(string name, string& value)
{
    if (name == "address")
        value = getString(address);
    else if (name == "bank")
        value = getString(bank);
    else
        return false;

    return true;
}

bool AERamFactor::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
    }
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "ram") {
        if (ram)
            ram->removeObserver(this, RAM_SIZE_DID_CHANGE);
        ram = ref;
        if (ram)
            ram->addObserver(this, RAM_SIZE_DID_CHANGE);
    }
    else
        return false;
    
    return true;
}

bool AERamFactor::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    OECheckComponent(bankSwitcher);
    OECheckComponent(ram);
    
    if (ram) {
        string size;
        if (!ram->getValue("size", size)) {
            logMessage("RamFactor cannot determine RAM size");
            return false;
        }
        hiOrF = getOELong(size) < 0x100000;
    }
    
    return true;
}

void AERamFactor::dispose()
{
}

void AERamFactor::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus) {
        ControlBusPowerState powerState = *((ControlBusPowerState *)data);
        
        if (powerState == CONTROLBUS_POWERSTATE_OFF)
        {
            address = 0;
            bank = 0;
        }
    } else if (sender == ram) {
        size_t size = *((size_t *)data);
        hiOrF = size < 0x100000;
    }
}

OEChar AERamFactor::read(OEAddress addr)
{
    switch (addr & 0xf) {
        case 0x0:
            return OEChar(address);
        case 0x1:
            return OEChar(address>>8);
        case 0x2:
            return hiOrF ? (OEChar(address>>16) | 0xF0) : OEChar(address>>16);
        case 0x3:
            return ram->read(incAddr());
        case 0xf:
            return bank;
        default:
            return floatingBus->read(0);
    }
}

void AERamFactor::write(OEAddress addr, OEChar value)
{
    switch (addr & 0xf) {
        case 0x0:
            address = (address&0xFFFF00) | OEAddress(value);
            break;
        case 0x1:
            address = (address&0xFF00FF) | (OEAddress(value) << 8);
            break;
        case 0x2:
            address = (address&0x00FFFF) | (OEAddress(value) << 16);
            break;
        case 0x3:
            ram->write(address, incAddr());
            break;
        case 0xf:
            setFirmwareBank(value);
            break;
    }
}

OEAddress AERamFactor::incAddr()
{
    OEAddress curr = address;
    address = (address+1)&0xFFFFFF;
    return curr;
}

void AERamFactor::setFirmwareBank(OEChar value)
{
    if (bank == value)
        return;

    if ((bank&1) == (value&1)) {
        bank = value;
        return;
    }

    bank = value;
    updateBankOffset();
}

void AERamFactor::updateBankOffset()
{
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0000;
    offsetMap.endAddress = 0xfff;
    offsetMap.offset = OEGetBit(bank,0) ? 0x1000 : 0x0000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}
