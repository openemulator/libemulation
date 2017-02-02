
/**
 * libemulation
 * Apple IIe MMU softswitch handler.
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls MMU (and IOU) softswitch setting/resetting/reading.
 */

#include "AppleIIEMMU.h"

AppleIIEMMU::AppleIIEMMU()
{
    controlBus = NULL;
	floatingBus = NULL;
    video = NULL;
}

bool AppleIIEMMU::setValue(string name, string value)
{
	return false;
}

bool AppleIIEMMU::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
        controlBus = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
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
    
    return true;
}

void AppleIIEMMU::update()
{
}

bool AppleIIEMMU::postMessage(OEComponent *sender, int message, void *data)
{
	return false;
}

OEChar AppleIIEMMU::read(OEAddress address)
{
	return floatingBus->read(address);
}

void AppleIIEMMU::write(OEAddress address, OEChar value)
{
}
