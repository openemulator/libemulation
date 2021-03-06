
/**
 * libemulation
 * OEComponent
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the base emulation component
 */

#include "math.h"
#include <sstream>

#include "OEComponent.h"

OEComponent::~OEComponent()
{
}

bool OEComponent::setValue(string name, string value)
{
    return false;
}

bool OEComponent::getValue(string name, string &value)
{
    return false;
}

bool OEComponent::setRef(string name, OEComponent *ref)
{
    return false;
}

bool OEComponent::setData(string name, OEData *data)
{
    return false;
}

bool OEComponent::getData(string name, OEData **data)
{
    return false;
}

bool OEComponent::init()
{
    return true;
}

void OEComponent::update()
{
    return;
}

void OEComponent::dispose()
{
    return;
}

bool OEComponent::postMessage(OEComponent *sender, int message, void *data)
{
    return false;
}

bool OEComponent::addObserver(OEComponent *observer, int notification)
{
    if (observer)
        observers[notification].push_back(observer);
    
    return true;
}

bool OEComponent::removeObserver(OEComponent *observer, int notification)
{
    OEComponents::iterator first = observers[notification].begin();
    OEComponents::iterator last = observers[notification].end();
    OEComponents::iterator i = remove(first, last, observer);
    
    if (i != last)
        observers[notification].erase(i, last);
    
    return (i != last);
}

void OEComponent::postNotification(OEComponent *sender, int notification, void *data)
{
    for (size_t i = 0; i < observers[notification].size(); i++)
        observers[notification][i]->notify(this, notification, data);
}

void OEComponent::notify(OEComponent *sender, int notification, void *data)
{
}

OEChar OEComponent::read(OEAddress address)
{
    return 0;
}

void OEComponent::write(OEAddress address, OEChar value)
{
}

OEShort OEComponent::read16(OEAddress address)
{
    return 0;
}

void OEComponent::write16(OEAddress address, OEShort value)
{
}

OEInt OEComponent::read32(OEAddress address)
{
    return 0;
}

void OEComponent::write32(OEAddress address, OEInt value)
{
}

OELong OEComponent::read64(OEAddress address)
{
    return 0;
}

void OEComponent::write64(OEAddress address, OELong value)
{
}
