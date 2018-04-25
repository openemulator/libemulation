
/**
 * libemulator
 * Apple IIe Keyboard
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple IIe keyboard
 */

#ifndef _APPLEIIEKEYBOARD_H
#define _APPLEIIEKEYBOARD_H

#include "OEComponent.h"
#include "AppleIIKeyboard.h"

#include "CanvasInterface.h"

#include <queue>

class AppleIIEKeyboard : public OEComponent
{
public:
    AppleIIEKeyboard();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool init();
	void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
protected:
    OEComponent *controlBus;
    OEComponent *floatingBus;
	OEComponent *monitor;
    
    OEInt pressedKeypadKeyCount;
    
    OEChar keyLatch;
    bool keyStrobe;
    bool anyKeyDown;
    
    virtual void updateKeyFlags();
    virtual void sendKey(CanvasUnicodeChar key);
    void setKeyStrobe(bool value);
    virtual void setReset(bool value);
    virtual void setAltReset(bool value);
    
    void emptyPasteBuffer();
    
private:
    OEComponent *gamePort;
    
    AppleIIKeyboardState state;
    OEInt stateUsageId;
    
    queue<OEChar> pasteBuffer;
    
    void paste(wstring *s);
};

#endif
