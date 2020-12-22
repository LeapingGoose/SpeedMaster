#ifndef EZ_OLED_COMP_H
#define EZ_OLED_COMP_H

/**
*
*		@todo Reorganize private and public
*		@todo Not all actions affect the children and they should. Wire them up.
*		id is really the pool position
*		We shouldn't need to know about pools in the actual component.
*
**/

// Pulling in the actual task scheduler causes conflicts. This is all we need.ß
#include <TaskSchedulerDeclarations.h>
#include "EzOledShared.h"
// #include <SSD1306Wire.h>
#include "../EzLib/EzLib.h"
#include <EzDebug.h>

// Forward declaration
class EzOled;

// Screen coordinates based off of relative ones.
// Position relative to the screen.
#define scrX(relativeX) relativeX+_x+(_parent != NULL ? _parent->screenX() : 0)
#define scrY(relativeY) relativeY+_y+(_parent != NULL ? _parent->screenY() : 0)

class EzOledComp {

  public:
    EzOledComp();
    EzOledComp(EzOled *eztf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    EzOledComp(EzOled *eztf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*onBtnAction)(EzOledComp* comp, BTN_ACTION action), bool isVisible = false);
    EzOledComp(EzOled *eztf, void (*drawFunc)(EzOledComp* comp, BTN_ACTION action));
    EzOledComp(EzOled *eztf);
    ~EzOledComp();

    void 						setup(EzOled *eztf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    void 						setup(EzOled *eztf, EzOledComp* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    void 						setup(EzOled *eztf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*onBtnAction)(EzOledComp* comp, BTN_ACTION action), bool isVisible = false);
    void 						setup(EzOled *eztf, void (*drawFunc)(EzOledComp* comp, COMP_ACT action));
    void 						setup(EzOled *eztf);
    EzOledComp* 		createChild(int16_t x, int16_t y, int16_t width, int16_t height, void (*drawFunc)(	EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    EzOledComp* 		createChild(COMP_TYPE type, int16_t x, int16_t y, int16_t width, int16_t height, void (*drawFunc)(	EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    void						callback(EzOledComp* comp, COMP_ACT action);
    void						renderAsButton();
    void						renderAsWindow();
    void						renderAsWindowHdrText();
    void						renderAsWindowBackBtn();
    void						renderAsWindowCancelBtn();
    void						renderAsWindowHeader();
    void						action(COMP_ACT action, bool includeChildren = false);
    COMP_ACT				action();
    bool 						visible();
    bool 						enabled();
    void 						enabled(bool enable, bool includeChildren = true);
    bool						suspendDrawing();

    // Parent
    EzOledComp* 		parent(EzOledComp* parentComp);
    EzOledComp* 		parent();
    bool 						parentEnabled();
    bool 						parentVisible();

    // Child
    EzOledComp* 		getChild(uint8_t childNum);
    EzOledComp*			addChild(EzOledComp* childComp);
    uint8_t					childCount();
    void 						removeChild(EzOledComp* child);
    void 						removeLastChild();

    //	Screen 
    uint16_t 				x();
    uint16_t 				y();
    uint16_t 				screenX();
    uint16_t 				screenY();
    uint16_t 				screenXRight();
    uint16_t 				screenYBottom();
    uint16_t 				width();
    uint16_t 				height();
    void						draw();
    void            display();

    // Used for labels, headers and good for identifying in debug.
    const char*			text();
    void						text(const char* text);
    void						clickCancelsWindow(bool cancels);
    bool						clickCancelsWindow();
    void 						drawRect(int16_t x, int16_t y, int16_t w, int16_t h);
    void 						fillRect(int16_t x, int16_t y, int16_t w, int16_t h);
    void 						clear();
    void 						fill(uint16_t offset = 0, uint8_t radius = 0);
    void 						drawPixel(int16_t x, int16_t y);
    void 						drawOutline(uint16_t thickness, uint8_t radius = 0, uint16_t offset = 0);
    void 						drawOutlineX(uint16_t thickness, int8_t radius = 0, int16_t offsetTop = 0, int16_t offsetRight = 0, int16_t offsetBottom = 0, int16_t offsetLeft = 0);
    void 						drawCircle(int16_t x, int16_t y, int16_t r);
    void 						fillCircle(int16_t x, int16_t y, int16_t r);
    void 						drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t radius);
    void 						drawBitmap(int32_t x, int32_t y, const uint8_t* bitmap, int32_t width, int32_t height);
    void 						setCursor(int16_t x, int16_t y);
    void            setFont(const uint8_t* font);
    const uint8_t*  getFont();
    uint16_t				getFontHeight();
    uint16_t				getStringWidth(const char* str);
    uint16_t        print(const char* st, int32_t x, int32_t y, const uint8_t font[]);
    uint16_t 				print(const char* st, int32_t x, int32_t y);
    uint16_t 				print(const char* text);
    uint16_t 				printNumI(long num, int32_t x, int32_t y);
    uint16_t        printNumF(double num, byte dec, int32_t x, int32_t y);
    void						drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void						drawHLine(int16_t x, int16_t y, int16_t len);
    void						drawVLine(int16_t x, int16_t y, int16_t len);
    COMP_TYPE				type();
    void						type(COMP_TYPE type);
    uint16_t 				leftPos();
    uint16_t 				rightPos();
    uint16_t 				topPos();
    uint16_t 				bottomPos();
    uint16_t 				getCompPosX(COMP_POS xPos, int16_t compWidth);
    uint16_t 				getCompPosY(COMP_POS yPos, int16_t compHeight);
    int16_t					poolPos();
    void						poolPos(int16_t poolPos);
    bool						inUse();
    void						inUse(bool used);
    ON_STATE				on();
    void						on(ON_STATE onState);
    EzOledComp*		  windowPanel();
    void						windowPanel(EzOledComp* comp);
    bool						btnClickOnRelease();
    void						btnClickOnRelease(bool clickOnRelease);
    void						initialize();
    bool						recordStats();
    void						dbgOut();
    void            addTask(Task* task);
    void            removeTask();
    void            setDebug(bool on);

  private:

    // Memory hogs
    EzOledComp*    _children[EZTF_MAX_CHILDREN];
    const char*    _text = NULL;
    bool           _btnClickOnRelease = true;
    bool           _clickCancelsWindow = false;
    bool           _inUse = false; // True when not in pool.
    ON_STATE       _on = ON_STATE::NONE;
    bool           _visible = false;
    bool           _isHolding = false;
    bool           _enabled = true;
    int16_t        _poolPos = -1; // Position in the creators array
    int16_t        _x = 0;
    int16_t        _y = 0;
    int16_t        _width = 0;
    int16_t        _height = 0;
    uint8_t        _childCnt = 0;
    EzOled*        _scr = NULL;
    SSD1306Wire*   _tft = &__tft;
    Task*          _task = NULL;
    const uint8_t* _font = NULL;
    EzOledComp*    _wndPnl = NULL;
    EzOledComp*    _parent = NULL;
    COMP_TYPE      _type = COMP_TYPE::panel;
    COMP_ACT       _action = COMP_ACT::initialize;
    bool           _debug = false;

    void (*_drawFunc)(EzOledComp* comp, COMP_ACT action);
    void (*_onBtnAction)(EzOledComp* comp, BTN_ACTION action);
    void visible(bool visible, bool includeChildren = true);
    void callDraw(EzOledComp* comp, COMP_ACT action);

};
#endif