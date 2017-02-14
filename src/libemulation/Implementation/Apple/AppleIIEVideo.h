
/**
 * libemulator
 * Apple IIe Video
 * (C) 2017 by Zellyn Hunter (zellyn@gmail.com)
 * Original code (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates Apple IIe video
 */

#include "OEComponent.h"
#include "OEImage.h"

#include "ControlBusInterface.h"

class AppleIIEVideo : public OEComponent
{
public:
    AppleIIEVideo();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
	bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *controlBus;
    OEComponent *gamePort;
    OEComponent *mmu;
    OEComponent *monitor;
    
    // Settings
    OEInt model;
    OEInt revision;
    OEInt videoSystem;
	string characterSet;
    string characterRom;
    OEInt flashFrameNum;
    OEInt mode;
    
    bool revisionUpdated;
    bool videoSystemUpdated;
    bool characterRomUpdated;
    
    // Tables
    vector<OEIntPoint> pos;
    vector<OEIntPoint> count;
    
    vector<OEInt> textOffset;
    vector<OEInt> hiresOffset;
    
    // Memory
    OEComponent *vram0000;
    OEAddress vram0000Offset;
    OEComponent *vram1000;
    OEAddress vram1000Offset;
    OEComponent *vram2000;
    OEAddress vram2000Offset;
    OEComponent *vram4000;
    OEAddress vram4000Offset;

    OEComponent *vram0000Aux;
    OEAddress vram0000OffsetAux;
    OEComponent *vram1000Aux;
    OEAddress vram1000OffsetAux;
    OEComponent *vram2000Aux;
    OEAddress vram2000OffsetAux;
    OEComponent *vram4000Aux;
    OEAddress vram4000OffsetAux;

    OEData dummyMemory;
    
    OEChar *textMemory[2];
    OEChar *hblMemory[2];
    OEChar *hiresMemory[2];
    OEChar *textMemoryAux[2];
    OEChar *hblMemoryAux[2];
    OEChar *hiresMemoryAux[2];
    
    // Drawing
    bool videoEnabled;
    bool colorKiller;
    
    map<string, OEData> text40Font;
    map<string, OEData> text80Font;
    OEData loresFont;
    OEData hires40Font;
    OEData hires80Font;
    
    map<string, OEData> characterRoms;
    OEData currentCharacterRom;
    OEData videoRomMaps;
    
    OEImage image;
    OEChar *imagep;
    OEInt imageWidth;
    bool imageModified;
    
    void (AppleIIEVideo::*draw)(OESInt y, OESInt x0, OESInt x1);
    OEChar *drawMemory1;
    OEChar *drawMemory2;
    OEChar *drawFont;
    
    // Timing
    OEInt vertTotal;
    OEInt vertStart;
    
    OELong frameStart;
    OEInt frameCycleNum;
    
    OEInt currentTimer;
    OELong lastCycles;
    OEInt pendingCycles;
    
    bool flash;
    OEInt flashCount;
    
    // State
    ControlBusPowerState powerState;
    bool monitorConnected;
    OEInt videoInhibitCount;
    bool an2;
    bool an3;
    
    void initOffsets();
    
    bool loadTextFont(string name, OEData *data);
    void buildLoresFont();
    void buildHires40Font();
    void buildHires80Font();
    
    void buildVideoRomMaps();
    
    void initVideoRAM(OEComponent *ram, OEAddress &start);
    void updateImage();
    void updateClockFrequency();
    
    void updateMonitorConnected();
    
    void setMode(OEInt mask, bool value);
    void configureDraw();
    void drawText40Line(OESInt y, OESInt x0, OESInt x1);
    void drawText80Line(OESInt y, OESInt x0, OESInt x1);
    void drawLores40Line(OESInt y, OESInt x0, OESInt x1);
    void drawHires40Line(OESInt y, OESInt x0, OESInt x1);
    void drawHires80Line(OESInt y, OESInt x0, OESInt x1);
    void newDrawText40Line(OESInt y, OESInt x0, OESInt x1);
    void newDrawText80Line(OESInt y, OESInt x0, OESInt x1);
    void newDrawLores40Line(OESInt y, OESInt x0, OESInt x1);
    void newDrawLores80Line(OESInt y, OESInt x0, OESInt x1);
    void newDrawHires40Line(OESInt y, OESInt x0, OESInt x1);
    void newDrawHires80Line(OESInt y, OESInt x0, OESInt x1);
    void updateVideoEnabled();
    void refreshVideo();
    void updateVideo();
    
    void updateTiming();
    void scheduleNextTimer(OESLong cycles);
    OEIntPoint getCount();
    OEChar readFloatingBus();
    
    void copy(wstring *s);
    
    // Adding IIe 80col support
    bool altchrset;
    bool _80store;

    void set80store(bool new80store);

    OEChar *romMap;
    
    OEInt romMapOffset(OEChar value, OESInt y, OESInt x, bool graphics, bool hgr);
};
