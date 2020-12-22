// @todo There is a lot of room for performance imporovements here. Inlining to start.

#ifndef EZ_OLED_H
#define EZ_OLED_H

#include <FS.h>
#include <SPIFFS.h>
#include "EzOledShared.h"
#include "EzOledComp.h"
#include <pgmspace.h>
#include <EzDebug.h>

#define COMP_ACT_REDRAW_ALL (COMP_ACT::initialize | COMP_ACT::enabled | COMP_ACT::visible | COMP_ACT::releasedSoft | COMP_ACT::releasedHard | COMP_ACT::refresh)

// Screen size
#define TS_MIN_Y 0
#define TS_MAX_Y 63
#define TS_MIN_X 0
#define TS_MAX_X 127

// Arbitrary value to use as no value.
#define NO_VALUE -1000

// A "window" is roughly thought of as a full screen panel. May refactor naming.
typedef struct cmp_window_t {
  // We'll hold the window component here but it will only exist when in use.
  EzOledComp* component;
  // The function that will generate the windows panel component.
  EzOledComp* (*createWindowPanelFunc)();
  const char* header = NULL;
} cmp_window_t;

class EzOled {

  public:
    EzOled();
    ~EzOled();
    void                 setup(byte orientation=LANDSCAPE);
    void                 loop();
    EzOledComp*          createComp(int16_t x, int16_t y, int16_t width, int16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    EzOledComp*          createComp(EzOledComp* parent, int16_t x, int16_t y, int16_t width,int16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    EzOledComp*          createComp(COMP_TYPE type, int16_t x, int16_t y, int16_t width,int16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible = false);
    EzOledComp*          createWindowPanel(void (*drawFunc)(EzOledComp* comp, COMP_ACT action), const char* header);
    EzOledComp*          getCompFromPool();
    EzOledComp*          createEmptyWindow(bool useCancelBtn = false);
    SSD1306Wire*         tft();
    void                 baseComp(EzOledComp* baseComp);
    EzOledComp*          baseComp();
    EzOledComp*          currComp();
    void                 returnToPool(EzOledComp* comp);
    void                 drawWindow(EzOledComp* comp, COMP_ACT action);
    void                 drawWindowHdrBar(EzOledComp* comp, COMP_ACT action);
    void                 drawWindowCloseBtn(EzOledComp* comp, COMP_ACT action);
    void                 showWindow(EzOledComp* (*createWindowPanelFunc)(), bool useCancelBtn = false);
    void                 closeWindow(bool cancel=false);
    void                 setFont(const uint8_t* font);
    void                 fontWndHdr(const uint8_t* font);
    const uint8_t*       fontWndHdr();
    const char*          currWindowHdr();
    const char*          newWindowHdr();
    EzOledComp*          currWindow();
    void                 fontBtn(const uint8_t *font);
    const uint8_t*       fontBtn();
    void                 setTextColor(uint16_t fg, uint16_t bg);
    void                 closeBtnIcon(const uint8_t* icon);
    const uint8_t*       closeBtnIcon();
    void                 cancelBtnIcon(const uint8_t* icon);
    const uint8_t*       cancelBtnIcon();
    void                 fontText(const uint8_t* font);
    const uint8_t*       fontText();
    int16_t              screenWidth();
    int16_t              screenHeight();
    byte                 orientation();
    void                 reverseX();
    void                 reverseY();
    void                 clearScreen();

#if(EZ_DEBUG_ON)
    char*                actionAsChr(COMP_ACT action);
#endif

  private:
    /**
     * Variables
     */
#if(EZ_DEBUG_ON)
    char                 _tmpActionText[15] = ""; // Used with actionAsChr(...)
#endif
    byte                 _orientation = LANDSCAPE;
    uint8_t              _ezWindowsCnt = 0;
    uint8_t              _windowsCreatedCnt = 0;
    uint8_t              _wndStackCreatedCnt = 0;
    uint8_t              _compPoolPos = 0;
    uint16_t             _disabledCompCnt = 0;
    uint16_t             _compsInUse = 0;
    uint16_t             _holdDurMs = 1250;
    int16_t              _minX = TS_MIN_X;
    int16_t              _maxX = TS_MAX_X;
    int16_t              _minY = TS_MIN_Y;
    int16_t              _maxY = TS_MAX_Y;
    int16_t              _toneDur = 100;
    int16_t              _toneLevel = 500;
    bool                 _ignoreNextRelease = false;
    const uint8_t*       _fontWndHdr = nullptr;
    const char*                _newHeaderText;
    const uint8_t*       _fontBtn = nullptr;
    const uint8_t*       _fontText = nullptr;
    EzOledComp*          _currWindow = NULL;
    EzOledComp*          _baseComp = NULL;
    EzOledComp*          _currBaseComp = NULL;
    const char*          _currHeader = NULL;
    const uint8_t*       _testBtnIcon;
    const uint8_t*       _closeBtnIcon;
    const uint8_t*       _cancelBtnIcon;
    cmp_window_t         _wndStack[DISP_MAX_WINDOWS-1];
    SSD1306Wire*         _tft = &__tft;
    EzOledComp*          _tmpDisabledComps[EZTF_MAX_COMPONENTS];
    EzOledComp           _compPool[EZTF_MAX_COMPONENTS];

    /**
     * Prototypes
     */
    
};
#endif