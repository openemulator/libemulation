/**
 * libemulation
 * Don't Ask Software S.A.M. DAC Card
 * (C) 2016 by Zellyn Hunter (zellyn@gmail.com)
 * Released under the GPL
 *
 * Controls a Don't Ask Software Digital Analog Converter Card
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class SAMDACCard : public OEComponent
{
public:
    SAMDACCard();

    void notify(OEComponent *sender, int notification, void *data);

    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    void dispose();

    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);

private:
    OEComponent *floatingBus;
    OEComponent *audioCodec;

    float volume;
    OESShort outputLevel;

    OELong lastCycles;
    bool relaxationState;
};
