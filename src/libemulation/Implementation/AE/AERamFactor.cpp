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
    size = 0;
    hiOr = 0;
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
        string sizeStr;
        if (!ram->getValue("size", sizeStr)) {
            logMessage("RamFactor cannot determine RAM size");
            return false;
        }
        size = getOELong(sizeStr);
        hiOr = (size < 0x100000) ? 0xF00000 : 0;
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
        size = *((size_t *)data);
        hiOr = (size < 0x100000) ? 0xF00000 : 0;
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
            return OEChar(address>>16);
        case 0x3:
        {
            OEAddress cur = incAddr();
            if (cur < size)
                return ram->read(cur);
            else
                return floatingBus->read(0);
        }
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
        {
            OEAddress cur = incAddr();
            if (cur < size)
                ram->write(cur, value);
            return;
        }
        case 0xf:
            setFirmwareBank(value);
            break;
    }
}

void AERamFactor::setAddressByte(int n, OEChar value) {
    if (n==2) {
        address = (address&0x00FFFF) | (OEAddress(value) << 16) | hiOr;
        return;
    }
    // “Whenever the lower or middle address byte changes from a
    // value with bit 7 = 1 to one with bit 7 = 0, the next higher
    // byte is automatically incremented. This means that you should
    // always load the bytes in the order low-middle-high, and
    // always load all three of them. (Unless, of course, you are
    // sure of the previous contents and can be sure you will get
    // predictably correct results.)”
    OEChar old = getAddressByte(n);
    if (OEGetBit(old, 0x80) && !OEGetBit(value, 0x80)) {
        setAddressByte(n+1, getAddressByte(n+1)+1);
    }
    if (n==0)
        address = (address&0xFFFF00) | OEAddress(value);
    else
        address = (address&0xFF00FF) | (OEAddress(value) << 8);
}

OEChar AERamFactor::getAddressByte(int n) {
    return OEChar(address >> (8*n));
}

OEAddress AERamFactor::incAddr()
{
    OEAddress curr = address;
    address = ((address+1)&0xFFFFFF) | hiOr;
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
    offsetMap.offset = OEGetBit(bank,1) ? 0x1000 : 0x0000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}
