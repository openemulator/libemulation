
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard
 */

#include "AppleIIEKeyboard.h"

#include "ControlBusInterface.h"

#include "AppleIIInterface.h"

AppleIIEKeyboard::AppleIIEKeyboard()
{
    controlBus = NULL;
    floatingBus = NULL;
    gamePort = NULL;
    monitor = NULL;
    
    pressedKeypadKeyCount = 0;
    state = APPLEIIKEYBOARD_STATE_NORMAL;
    
    keyLatch = 0;
    keyStrobe = false;
}

bool AppleIIEKeyboard::setValue(string name, string value)
{
	return false;
}

bool AppleIIEKeyboard::getValue(string name, string& value)
{
	return false;
}

bool AppleIIEKeyboard::setRef(string name, OEComponent *ref)
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
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "gamePort")
        gamePort = ref;
	else if (name == "monitor")
    {
        if (monitor)
        {
            monitor->removeObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->removeObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
            monitor->removeObserver(this, CANVAS_DID_PASTE);
        }
        monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->addObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
            monitor->addObserver(this, CANVAS_DID_PASTE);
        }
    }
	else
		return false;
	
	return true;
}

bool AppleIIEKeyboard::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    
    update();
    
    return true;
}

void AppleIIEKeyboard::update()
{
    updateKeyFlags();
}

void AppleIIEKeyboard::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                if (*((ControlBusPowerState *)data) == CONTROLBUS_POWERSTATE_ON)
                {
                    keyLatch = 0;
                    setKeyStrobe(false);
                }
                
                break;
                
            case CONTROLBUS_RESET_DID_ASSERT:
                // Clear pastebuffer
                while (!pasteBuffer.empty())
                    pasteBuffer.pop();
                
                break;
        }
    }
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_UNICODECHAR_WAS_SENT:
            {
                if (state != APPLEIIKEYBOARD_STATE_NORMAL)
                    break;
                
                if (!pasteBuffer.empty())
                    break;
                
                sendKey(*((CanvasUnicodeChar *)data));
                
                break;
            }
            case CANVAS_KEYBOARD_DID_CHANGE:
            {
                updateKeyFlags();
                
                CanvasHIDEvent *hidEvent = (CanvasHIDEvent *)data;
                
                if ((hidEvent->usageId >= CANVAS_KP_NUMLOCK) &&
                    (hidEvent->usageId <= CANVAS_KP_PERIOD))
                    pressedKeypadKeyCount += hidEvent->value ? 1 : -1;
                
                switch (state)
                {
                    case APPLEIIKEYBOARD_STATE_NORMAL:
                        // React on key down
                        if (!hidEvent->value)
                            break;
                        
                        if (hidEvent->usageId == CANVAS_K_BACKSPACE)
                        {
                            stateUsageId = hidEvent->usageId;
                            
                            CanvasKeyboardFlags flags;
                            
                            monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
                            
                            if (OEGetBit(flags, CANVAS_KF_CONTROL) &&
                                OEGetBit(flags, CANVAS_KF_GUI))
                            {
                                ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_OFF;
                                controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                                
                                state = APPLEIIKEYBOARD_STATE_RESTART;
                            }
                            else if (OEGetBit(flags, CANVAS_KF_CONTROL) &&
                                     OEGetBit(flags, CANVAS_KF_ALT))
                            {
                                setAltReset(true);
                                
                                state = APPLEIIKEYBOARD_STATE_ALTRESET;
                            }
                            else if (OEGetBit(flags, CANVAS_KF_CONTROL))
                            {
                                setReset(true);
                                
                                state = APPLEIIKEYBOARD_STATE_RESET;
                            }
                        }
                        else if (hidEvent->usageId == CANVAS_K_F12)
                        {
                            stateUsageId = hidEvent->usageId;
                            
                            CanvasKeyboardFlags flags;
                            
                            monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
                            
                            if (OEGetBit(flags, CANVAS_KF_CONTROL) &&
                                OEGetBit(flags, CANVAS_KF_GUI))
                            {
                                ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_OFF;
                                controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                                
                                state = APPLEIIKEYBOARD_STATE_RESTART;
                            }
                            else if (OEGetBit(flags, CANVAS_KF_CONTROL) &&
                                     OEGetBit(flags, CANVAS_KF_ALT))
                            {
                                setAltReset(true);
                                
                                state = APPLEIIKEYBOARD_STATE_ALTRESET;
                            }
                            else if (OEGetBit(flags, CANVAS_KF_CONTROL))
                            {
                                setReset(true);
                                
                                state = APPLEIIKEYBOARD_STATE_RESET;
                            }
                        }
                        break;
                        
                    case APPLEIIKEYBOARD_STATE_RESET:
                        // React on key up
                        if (hidEvent->value)
                            break;
                        
                        if (hidEvent->usageId == stateUsageId)
                        {
                            setReset(false);
                            
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
                        
                    case APPLEIIKEYBOARD_STATE_ALTRESET:
                        // React on key up
                        if (hidEvent->value)
                            break;
                        
                        if (hidEvent->usageId == stateUsageId)
                        {
                            setAltReset(false);
                            
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
                        
                    case APPLEIIKEYBOARD_STATE_RESTART:
                        // React on key up
                        if (hidEvent->value)
                            break;
                        
                        if (hidEvent->usageId == stateUsageId)
                        {
                            ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_ON;
                            controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                            
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
                }
                
                break;
            }
            case CANVAS_DID_PASTE:
                paste((wstring *)data);
                
                break;
        }
    }
}

OEChar AppleIIEKeyboard::read(OEAddress address)
{
    if (address & 0x10)
    {
        setKeyStrobe(false);
        
        emptyPasteBuffer();
        
        return floatingBus->read(address);
    }
    
    return (keyStrobe << 7) | (keyLatch & 0x7f);
}

void AppleIIEKeyboard::write(OEAddress address, OEChar value)
{
    read(address);
}

void AppleIIEKeyboard::updateKeyFlags()
{
    if (!monitor || !gamePort)
        return;

    CanvasKeyboardFlags flags;

    monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);

    bool openApple = OEGetBit(flags, CANVAS_KF_LEFTALT);
    bool closedApple = OEGetBit(flags, CANVAS_KF_RIGHTALT);

    gamePort->postMessage(this, APPLEII_SET_PB0, &openApple);
    gamePort->postMessage(this, APPLEII_SET_PB1, &closedApple);
}

void AppleIIEKeyboard::sendKey(CanvasUnicodeChar key)
{
    if (key == CANVAS_U_LEFT)
        key = 0x8;
    else if (key == CANVAS_U_RIGHT)
        key = 0x15;
    else if (key == CANVAS_U_UP)
        key = 0x0b;
    else if (key == CANVAS_U_DOWN)
        key = 0x0a;
    else if (key >= 0x80)
        return;
    
    keyLatch = key;
    setKeyStrobe(true);
}

void AppleIIEKeyboard::setKeyStrobe(bool value)
{
    bool oldKeyStrobe = keyStrobe;
    
    keyStrobe = value;
    
    if (keyStrobe != oldKeyStrobe)
        postNotification(this, APPLEII_KEYSTROBE_DID_CHANGE, &keyStrobe);
}

void AppleIIEKeyboard::setReset(bool value)
{
    if (value)
        controlBus->postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
    else
        controlBus->postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
}

void AppleIIEKeyboard::setAltReset(bool value)
{
    if (value)
        controlBus->postMessage(this, CONTROLBUS_ASSERT_NMI, NULL);
    else
        controlBus->postMessage(this, CONTROLBUS_CLEAR_NMI, NULL);
}

void AppleIIEKeyboard::paste(wstring *s)
{
    for (OEInt i = 0; i < s->size(); i++)
        pasteBuffer.push(s->at(i));
    
    emptyPasteBuffer();
}

void AppleIIEKeyboard::emptyPasteBuffer()
{
    while (!keyStrobe && !pasteBuffer.empty())
    {
        OEInt c = pasteBuffer.front();
        
        if (c == '\n')
            c = '\r';
        else if (c == '\r')
            continue;
        
        sendKey(c);
        
        pasteBuffer.pop();
    }
}
