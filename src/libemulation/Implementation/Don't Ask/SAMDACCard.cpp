/**
 * libemulation
 * Don't Ask Software S.A.M. DAC Card
 * (C) 2016 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls a Don't Ask Software Digital Analog Converter Card
 */

#include <iostream>

#include "SAMDACCard.h"

#include "AppleIIInterface.h"
#include "MemoryInterface.h"
#include "AudioInterface.h"

SAMDACCard::SAMDACCard()
{
    floatingBus = NULL;
}

bool SAMDACCard::setValue(string name, string value)
{
    if (name == "volume")
        volume = getFloat(value);
    else
        return false;

    return true;
}

bool SAMDACCard::getValue(string name, string& value)
{
    if (name == "volume")
        value = getString(volume);
    else
        return false;
    
    return true;
}

bool SAMDACCard::setRef(string name, OEComponent *ref)
{
    if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "audioCodec")
        audioCodec = ref;
    else
        return false;

    return true;
}

bool SAMDACCard::init()
{
    OECheckComponent(floatingBus);
    
    update();
    
    return true;
}

void SAMDACCard::update()
{
    outputLevel = 16384 * getLevelFromVolume(volume);
}

void SAMDACCard::dispose()
{
}

OEChar SAMDACCard::read(OEAddress address)
{
    return floatingBus->read(address);
}

void SAMDACCard::write(OEAddress address, OEChar value)
{
    // Real board probably allows writes to any address... ?
    // if ((address & 0xf) != 0) return;
    
    float scaledValue = value / 255.0;
    OEShort outValue = outputLevel * scaledValue;

    audioCodec->write16(0, outValue);
    audioCodec->write16(1, outValue);
}

void SAMDACCard::notify(OEComponent *sender, int notification, void *data)
{
}