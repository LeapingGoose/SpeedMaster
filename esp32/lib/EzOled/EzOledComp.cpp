/**
*
*		@todo Don't let parent hijack the state. Do as the parent asks but maintain our internal state.
*		@todo enabled and visible are too intertwined
*
**/

#include "EzOledComp.h"
#include "EzOled.h"

EzOledComp::EzOledComp() {
  initialize();
}

// Used when creating windows
EzOledComp::EzOledComp(EzOled *eztf) {
  setup(eztf);
}

EzOledComp::~EzOledComp() {
  removeTask();
}

void EzOledComp::removeTask() {
  if (_task != NULL) {
    _task->disable();
    _task = NULL;
  }
}

// Used when creating regular panels
void EzOledComp::setup(EzOled *eztf, EzOledComp* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible) {
  _parent = parent;
  setup(eztf, x, y, width, height, drawFunc, isVisible);
}

void EzOledComp::setup(EzOled *eztf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible) {
  _type = COMP_TYPE::panel;
  _x = x;
  _y = y;
  _width = width;
  _height = height;
  _drawFunc = drawFunc;
  _scr = eztf;
  visible(isVisible);
}

// Used when creating Windows
void EzOledComp::setup(EzOled *eztf, void (*drawFunc)(EzOledComp* comp, COMP_ACT action)) {
  _x = 0;
  _y = DISP_WND_HDR_HEIGHT;
  _width = _scr->screenWidth() - 1;
  _height = _scr->screenHeight() - 1 - DISP_WND_HDR_HEIGHT;
  _drawFunc = drawFunc;
  // _tft = _tft = eztf->tft();
  _scr = eztf;
  visible(false);
}

// Minimal setup.
void EzOledComp::setup(EzOled *eztf) {
  _type = COMP_TYPE::window;
  _x = 0;
  _y = DISP_WND_HDR_HEIGHT;
  _width = _scr->screenWidth() - 1;
  _height = _scr->screenHeight() - 1 - DISP_WND_HDR_HEIGHT;
  _drawFunc = NULL;
  // _tft = _tft = eztf->tft();
  _scr = eztf;
  visible(false);
}

void EzOledComp::callback(EzOledComp* comp, COMP_ACT action) {
  if (_drawFunc != NULL) {
    callDraw(comp, action);
  }
}

void EzOledComp::display() {
  _tft->display();
}

void EzOledComp::draw() {
  // We don't actually want to draw anything special here, just want to let the caller know what's going on.
  if (_action & (COMP_ACT::reset | COMP_ACT::close_wnd | COMP_ACT::cancel)) {
    if (_type == COMP_TYPE::window) {
      _wndPnl->callback(_wndPnl, _action);
    } else if (_drawFunc != NULL) {
      callDraw(this, _action);
    }
    return;
  }

  if ((visible() == false || enabled() == false)) {
    return;
  }

  if (_type == COMP_TYPE::window) {
    renderAsWindow();
  } else if (_type == COMP_TYPE::window_header) {
    renderAsWindowHeader();
  } else if (_type == COMP_TYPE::window_header_text) {
    renderAsWindowHdrText();
  } else if (_type == COMP_TYPE::window_back_btn) {
    renderAsWindowBackBtn();
  } else if (_type == COMP_TYPE::window_cancel_btn) {
    renderAsWindowCancelBtn();
  } else if (_drawFunc != NULL) {
    if (_clickCancelsWindow) {
      _scr->closeWindow(true);
    } else {
      callDraw(this, _action);
    }
  }
}

void EzOledComp::callDraw(EzOledComp* comp, COMP_ACT action) {
  _drawFunc(comp, action);
}

void EzOledComp::clickCancelsWindow(bool cancels) {
  _clickCancelsWindow = cancels;
}

bool EzOledComp::clickCancelsWindow() {
  return _clickCancelsWindow;
}

void EzOledComp::renderAsWindow() {
  if (_action & (COMP_ACT_REDRAW_ALL | COMP_ACT::invisible)) {
    clear();
  }
}

void EzOledComp::renderAsWindowHeader() {

  if (_action & (COMP_ACT::releasedHard | COMP_ACT::unhold)) {
    _scr->closeWindow();

  } else if (_action & COMP_ACT_REDRAW_ALL) {
    // The line separator between the header and the window content.
    drawHLine(0, _height - 1, _width - 1);
    print(_scr->newWindowHdr(), 0, 0);
    display();
  }
}

void EzOledComp::renderAsWindowHdrText() {
  if (_action & (COMP_ACT::releasedHard | COMP_ACT::unhold)) {
    _scr->closeWindow();
  } else if (_action & COMP_ACT_REDRAW_ALL) {
    
    setFont(_scr->fontWndHdr());
    print(_scr->currWindowHdr(), 1, height()/2 - getFontHeight()/2);
    display();
  }
}

void EzOledComp::renderAsWindowBackBtn() {
  if (_action & (COMP_ACT::releasedHard | COMP_ACT::unhold)) {
    _scr->closeWindow();
  } else if (_action & COMP_ACT_REDRAW_ALL) {
    // drawBitmap((DISP_BTN_CLOSE_WIDTH/2)-(DISP_ICON_WIDTH/2), (DISP_BTN_CLOSE_HEIGHT/2)-(DISP_ICON_HEIGHT/2), _scr->closeBtnIcon(), DISP_ICON_WIDTH, DISP_ICON_HEIGHT);
  }
}

void EzOledComp::renderAsWindowCancelBtn() {
  if (_action & (COMP_ACT::releasedHard | COMP_ACT::unhold)) {
    _scr->closeWindow(true);
  } else if (_action & COMP_ACT_REDRAW_ALL) {
    // drawBitmap((DISP_BTN_CLOSE_WIDTH/2)-(DISP_ICON_WIDTH/2), (DISP_BTN_CLOSE_HEIGHT/2)-(DISP_ICON_HEIGHT/2), _scr->cancelBtnIcon(), DISP_ICON_WIDTH, DISP_ICON_HEIGHT);
  }
}

void EzOledComp::renderAsButton() {
  if (_action & (COMP_ACT::initialize | COMP_ACT::enabled | COMP_ACT::visible | COMP_ACT::releasedSoft | COMP_ACT::releasedHard | COMP_ACT::refresh)) {
    if (_action & (COMP_ACT::releasedHard | COMP_ACT::releasedSoft)) {
      drawOutlineX(1, 0, 1, 1, 1, 1);
      if (_btnClickOnRelease == true) {
        _onBtnAction(this, BTN_ACTION::click);
      }
      display();
    } else {
      drawOutlineX(1);
      if (_text != NULL) {
        setFont(_scr->fontBtn());
        print(_text, width()/2 - getStringWidth(_text)/2, height()/2 - getFontHeight()/2);
      }
      display();
    }
  } else if (_action & COMP_ACT::justPressed) {
    drawOutlineX(1, 0, 1, 1, 1, 1);
    display();
    if (_btnClickOnRelease == false) {
      _onBtnAction(this, BTN_ACTION::click);
    }
  }	
}

void EzOledComp::setDebug(bool on) {
  _debug = on;
}

void EzOledComp::action(COMP_ACT action, bool includeChildren) {

  if (action & COMP_ACT::refresh || action != _action) {
    _action = action;
    bool drawn = true;

    if (action & COMP_ACT::refresh && _enabled == false) {
      return;
    }

    switch (action) {
    case COMP_ACT::enabled:
      enabled(true);
      if (_task != NULL) {
        _task->enable();
      }
      break;
    case COMP_ACT::disabled:
      enabled(false);
      if (_task != NULL) {
        _task->disable();
      }
      break;
    case COMP_ACT::visible:
      visible(true);
      if (_task != NULL) {
        _task->enable();
      }
      break;
    case COMP_ACT::invisible:
      visible(false);
      if (_task != NULL) {
        _task->disable();
      }
      break;
    default:
      drawn = false;
    }

    if (drawn == false) {
      // @todo I think most of these should end up as function calls
      // as they are in the switch above. These should migrate up there.
      switch (action)
      {
      case COMP_ACT::refresh:
        if (includeChildren == true) {
          for (uint8_t childNum = 0; childNum < _childCnt; childNum++) {
            _children[childNum]->action(action);
          }
        }
        break;
      case COMP_ACT::initialize:
      case COMP_ACT::invisible:
      case COMP_ACT::disabled:
      case COMP_ACT::enabled:
      case COMP_ACT::visible:
      case COMP_ACT::cancel:
      case COMP_ACT::close_wnd:
      case COMP_ACT::reset:
      case COMP_ACT::justPressed:
      case COMP_ACT::holding:
      case COMP_ACT::releasedSoft:
      case COMP_ACT::releasedHard:
      case COMP_ACT::touchable:
      case COMP_ACT::untouchable:
      case COMP_ACT::unhold:
      case COMP_ACT::none:
        break;
      }
      
      draw();

      if (action & (COMP_ACT::close_wnd | COMP_ACT::cancel | COMP_ACT::reset)) {
        if (_task != NULL) {
          _task->disable();
          _task = NULL;
        }
        _scr->returnToPool(this);
      }
    }
  }
}

EzOledComp* EzOledComp::parent(EzOledComp* parentComp) {
  _parent = parentComp;
  return parentComp;
}

EzOledComp* EzOledComp::addChild(EzOledComp* childComp) {
  childComp->parent(this);
  _childCnt++;
  _children[_childCnt - 1] = childComp;
  return childComp;
}

uint16_t EzOledComp::getCompPosX(COMP_POS xPos, int16_t compWidth) {
  uint16_t x = 0;
  if (xPos == COMP_POS::center) {
    x = width()/2 - compWidth/2;
  } else if (xPos == COMP_POS::left) {
    x = 0;
  } else if (xPos == COMP_POS::right) {
    x = width() - compWidth;
  }
  return x;
}

uint16_t EzOledComp::getCompPosY(COMP_POS yPos, int16_t compHeight) {
  uint16_t y = 0;
  if (yPos == COMP_POS::center) {
    y = height()/2 - compHeight/2;
  } else if (yPos == COMP_POS::top) {
    y = 0;
  } else if (yPos == COMP_POS::bottom) {
    y = height() - compHeight;
  }
  return y;
}

int16_t EzOledComp::poolPos() {
  return _poolPos;
};

void EzOledComp::poolPos(int16_t poolPos) {
  _poolPos = poolPos;
};

EzOledComp* EzOledComp::windowPanel() {
  return _wndPnl;
}

// @todo should make sure we don't get more than one call. Only want/need one.
void EzOledComp::windowPanel(EzOledComp* comp) {
  _wndPnl = comp;
  addChild(comp);	
}

const char* EzOledComp::text() {
  return _text;
}

void EzOledComp::text(const char* text) {
  _text = text;
}

EzOledComp* EzOledComp::createChild(int16_t x, int16_t y, int16_t width, int16_t height, void (*drawFunc)(	EzOledComp* comp, COMP_ACT action), bool isVisible) {
  return addChild(_scr->createComp(this, x, y, width, height, drawFunc, isVisible));
}

EzOledComp* EzOledComp::createChild(COMP_TYPE type, int16_t x, int16_t y, int16_t width, int16_t height, void (*drawFunc)(	EzOledComp* comp, COMP_ACT action), bool isVisible) {
  return addChild(_scr->createComp(type, x, y, width, height, drawFunc, isVisible));
}

void EzOledComp::removeChild(EzOledComp* child) {
  bool shift = false;
  for (uint8_t childNum = 0; childNum < _childCnt; childNum++) {
    // Remove the child and then shift the rest of the values
    // to fill where it was.
    if (_children[childNum] == child || shift) {
      shift = true;
      if (childNum == _childCnt - 1) {
        _scr->returnToPool(_children[childNum]);
        _children[childNum] = NULL;
        _childCnt--;
      } else {
        _children[childNum] = _children[childNum + 1];
      }
    }
  }
}

void EzOledComp::removeLastChild() {
  if (_childCnt > 0) {
    _scr->returnToPool(_children[_childCnt - 1]);
    _children[_childCnt - 1] = NULL;
    _childCnt--;
  }
}

// Children are accessed by number, their number is the order they were added.
EzOledComp* EzOledComp::getChild(uint8_t childNum) {
  return _children[childNum - 1];
}

uint8_t EzOledComp::childCount() {
  return _childCnt;
}

// Determine if the parent is active or note. If there is no parent then true.
bool EzOledComp::parentEnabled() {
  return _parent == NULL ? true : _parent->enabled();
}

bool EzOledComp::parentVisible() {
  return _parent == NULL ? true : _parent->visible();
}

void EzOledComp::initialize() {
  // If we were previously used, blow away the children.
  for (uint8_t childNum = 0; childNum < _childCnt; childNum++) {
    _children[childNum]->action(COMP_ACT::reset);
    _children[childNum] = NULL;
  }

  removeTask();
  _text               = NULL;
  _btnClickOnRelease  = true;
  _clickCancelsWindow = false;
  _inUse              = false; // True when not in pool.
  _on                 = ON_STATE::NONE;
  _visible            = false;
  _enabled            = true;
  _poolPos            = -1; // Position in the creators array
  _x                  = 0;
  _y                  = 0;
  _width              = 0;
  _height             = 0;
  _childCnt           = 0;
  _scr                = NULL;
  _task               = NULL;
  _font               = NULL;
  _wndPnl             = NULL;
  _parent             = NULL;
  _type               = COMP_TYPE::panel;
  _action             = COMP_ACT::initialize;
}

void EzOledComp::visible(bool isVisible, bool includeChildren) {
  if (_visible == isVisible) {
    return;
  }
  _visible = isVisible;
  _on = ON_STATE::NONE;
  draw();
  if (includeChildren == true) {
    for (uint8_t childNum = 0; childNum < _childCnt; childNum++) {
      _children[childNum]->action(isVisible ? COMP_ACT::visible : COMP_ACT::invisible);
    }
  }
}

bool EzOledComp::enabled() {
  return _enabled && parentEnabled();
}

bool EzOledComp::suspendDrawing() {
  return !enabled() || !visible();
}

bool EzOledComp::visible() {
  return _visible && parentVisible();
}

void EzOledComp::enabled(bool enable, bool includeChildren) {
  _enabled = enable;
  if (_enabled == true) {
    // @todo this is duplicated with an earlier setting in visible(). Need to clean up.
    _visible = true;
    draw();
  }
  if (includeChildren == true) {
    for (uint8_t childNum = 0; childNum < _childCnt; childNum++) {
      _children[childNum]->action(enable ? COMP_ACT::enabled : COMP_ACT::disabled);
    }
  }
}

void EzOledComp::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  _tft->drawLine(scrX(x0), scrY(y0), scrX(x1), scrY(y1));
}

void EzOledComp::drawHLine(int16_t x, int16_t y, int16_t len) {

  _tft->drawHorizontalLine(scrX(x) , scrY(y), len);
}

void EzOledComp::drawVLine(int16_t x, int16_t y, int16_t len) {
  _tft->drawVerticalLine(scrX(x) , scrY(y), len);
}

void EzOledComp::drawPixel(int16_t x, int16_t y) {
  _tft->setPixel(scrX(x) , scrY(y));
}

// Pass-throughs for the graphics management but only applies to the given EzOledComp.
void EzOledComp::drawRect(int16_t x, int16_t y, int16_t width, int16_t h) {
  _tft->drawRect(scrX(x), scrY(y), width, h);
}

void EzOledComp::fillRect(int16_t x, int16_t y, int16_t width, int16_t h) {
  _tft->fillRect(scrX(x), scrY(y), width, h);
}

void EzOledComp::clear() {
  // _tft->setColor(OLEDDISPLAY_COLOR::BLACK);
  _tft->setColor(OLEDDISPLAY_COLOR::BLACK);
  _tft->fillRect(_x, _y, _width, _height);
  _tft->setColor(OLEDDISPLAY_COLOR::WHITE);
}

void EzOledComp::fill(uint16_t offset, uint8_t radius) {
  if (radius == 0) {
    _tft->fillRect(scrX(0) + offset, scrY(0) + offset, _width - (offset * 2), _height - (offset * 2));
  } else {
    /** @todo Need to create fillRoundRect(...) function */
    _tft->fillRect(scrX(0) + offset, scrY(0) + offset, _width - (offset * 2), _height - (offset * 2));
  }
}

void EzOledComp::drawCircle(int16_t x, int16_t y, int16_t r) {
  _tft->drawCircle(scrX(x), scrY(y), r);
}

void EzOledComp::drawOutline(uint16_t thickness, uint8_t radius, uint16_t offset) {
  int32_t adjX = 0, adjY = 0, adjWidth = 0, adjHeight = 0;

  for (uint16_t n = 0; n < thickness; n++) {
    if (radius == 0) {
      drawRect(adjX + offset, adjY + offset, _width - adjWidth - (offset * 2), _height - adjHeight - (offset * 2));
    } else {
      drawRoundRect(adjX + offset,
                    adjY + offset,
                    _width - adjWidth - (offset * 2),
                    _height - adjHeight - (offset * 2),
                    radius);
    }
    adjX += 1;
    adjY += 1;
    adjWidth += 2;
    adjHeight += 2;
  }
}

// Control the offset of each line of the square. Also doesn't draw a side
// if it's offset is < 0.
// @todo fix this so when a line isn't drawn and the thickness is >1 you don't
// see the lines scewing.
void EzOledComp::drawOutlineX(uint16_t thickness, int8_t radius,
                                 int16_t offsetTop, int16_t offsetRight,
                                 int16_t offsetBottom, int16_t offsetLeft)
{
  // @todo Free up some of this memory. Don't need this many variables.
  int32_t adjX = 0, adjY = 0, adjWidth = 0, adjHeight = 0;
  int16_t osTop = offsetTop >= 0 ? offsetTop : 0;
  int16_t osBottom = offsetBottom >= 0 ? offsetBottom : 0;
  int16_t osLeft = offsetLeft >= 0 ? offsetLeft : 0;
  int16_t osRight = offsetRight >= 0 ? offsetRight : 0;
  bool drawTop = offsetTop >= 0;
  bool drawBottom = offsetBottom >= 0;
  bool drawLeft = offsetLeft >= 0;
  bool drawRight = offsetRight >= 0;

  for (uint16_t n = 0; n < thickness; n++) {
    if (radius == 0) {
      int16_t x = adjX + osLeft;
      int16_t y = adjY + osTop;
      int16_t w = _width - adjWidth - osRight - osLeft;
      int16_t h = _height - adjHeight - osBottom - osTop;
      if (drawTop)		{ drawHLine(x, y, w); }
      if (drawBottom)	{ drawHLine(x, y + h, w); }
      if (drawLeft)		{ drawVLine(x, y, h); }
      if (drawRight)	{ drawVLine(x + w, y, h); }
    } else {
      // @todo Add this back in but adjust the x, y stuff as above.
      int16_t x = adjX + osLeft;
      int16_t y = adjY + osTop;
      int16_t w = _width - adjWidth - osRight - osLeft;
      int16_t h = _height - adjHeight - osBottom - osTop;
      int16_t x2 = x + w;
      int16_t y2 = y + h;
      // @todo Will want to look into not drawing some of these
      // pixels depending on the negative offset values.
      drawPixel(x+1,y+1);
      drawPixel(x2-1,y+1);
      drawPixel(x+1,y2-1);
      drawPixel(x2-1,y2-1);
      if (drawTop)		{ drawHLine(x+2, y, x2-x-4); }
      if (drawBottom)	{ drawHLine(x+2, y2, x2-x-4); }
      if (drawLeft)		{ drawVLine(x, y+2, y2-y-4); }
      if (drawRight)	{ drawVLine(x2, y+2, y2-y-4); }
    }
    adjX += 1;
    adjY += 1;
    adjWidth += 2;
    adjHeight += 2;
  }
}

void EzOledComp::fillCircle(int16_t x, int16_t y, int16_t r) {
  _tft->fillCircle(scrX(x), scrY(y), r);
}

/** @note RADIUS IS NOT CURRENTLY WORKING. CURRENTLY DRAWS REGULAR RECT*/
void EzOledComp::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t radius) {
  drawRect(x, y, w, h);
  // _tft->drawRoundRect(scrX(x), scrY(y), width, h, radius);
}

void EzOledComp::drawBitmap(int32_t x, int32_t y, const uint8_t* bitmap, int32_t w, int32_t h) {
  _tft->drawFastImage(x, y, w, h, bitmap);
}

uint16_t EzOledComp::getStringWidth(const char* str) {
  return _tft->getStringWidth(str);
}

void EzOledComp::setFont(const uint8_t* font) {
  _font = font;
  _tft->setFont(font);
}

const uint8_t* EzOledComp::getFont() {
  return _font;
}

/** @todo Return the proper height */
uint16_t EzOledComp::getFontHeight() {
  return 24;
}

/** @todo Add this **/
// size_t Print::printf(const char *format, ...)
// {
//     char loc_buf[64];
//     char * temp = loc_buf;
//     va_list arg;
//     va_list copy;
//     va_start(arg, format);
//     va_copy(copy, arg);
//     int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
//     va_end(copy);
//     if(len < 0) {
//         va_end(arg);
//         return 0;
//     };
//     if(len >= sizeof(loc_buf)){
//         temp = (char*) malloc(len+1);
//         if(temp == NULL) {
//             va_end(arg);
//             return 0;
//         }
//         len = vsnprintf(temp, len+1, format, arg);
//     }
//     va_end(arg);
//     len = write((uint8_t*)temp, len);
//     if(temp != loc_buf){
//         free(temp);
//     }
//     return len;
// }

uint16_t EzOledComp::print(const char* st) {
  _tft->print(st);
  return _tft->getStringWidth(st);
}

uint16_t EzOledComp::print(const char* st, int32_t x, int32_t y) {
  _tft->drawString(scrX(x), scrY(y), st);
  return _tft->getStringWidth(st);
}

uint16_t EzOledComp::print(const char* st, int32_t x, int32_t y, const uint8_t font[]) {
  setFont(font);
  _tft->drawString(scrX(x), scrY(y), st);
  return _tft->getStringWidth(st);
}

uint16_t EzOledComp::printNumI(long num, int32_t x, int32_t y) {
  char str[12];
  return print(itoa(num, &(str[0]), 10), x, y);
}

// Returns pixel length.
// int16_t TFT_eSPI::drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY, uint8_t font)


uint16_t EzOledComp::printNumF(double floatNumber, byte dp, int32_t poX, int32_t poY) {
  char str[14];               // Array to contain decimal string
  uint8_t ptr = 0;            // Initialise pointer for array
  int8_t  digits = 1;         // Count the digits to avoid array overflow
  float rounding = 0.5;       // Round up down delta

  if (dp > 7) dp = 7; // Limit the size of decimal portion

  // Adjust the rounding value
  for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

  if (floatNumber < -rounding) {   // add sign, avoid adding - sign to 0.0!
    str[ptr++] = '-'; // Negative number
    str[ptr] = 0; // Put a null in the array as a precaution
    digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
    floatNumber = -floatNumber; // Make positive
  }

  floatNumber += rounding; // Round up or down

  // For error put ... in string and return (all TFT_eSPI library fonts contain . character)
  if (floatNumber >= 2147483647) {
    strcpy(str, "...");
    return print(str, scrX(poX), scrY(poY));
  }
  // No chance of overflow from here on

  // Get integer part
  uint32_t temp = (uint32_t)floatNumber;

  // Put integer part into array
  ltoa(temp, str + ptr, 10);

  // Find out where the null is to get the digit count loaded
  while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
  digits += ptr;                  // Count the digits

  str[ptr++] = '.'; // Add decimal point
  str[ptr] = '0';   // Add a dummy zero
  str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

  // Get the decimal portion
  floatNumber = floatNumber - temp;

  // Get decimal digits one by one and put in array
  // Limit digit count so we don't get a false sense of resolution
  uint8_t i = 0;
  while ((i < dp) && (digits < 9)) { // while (i < dp) for no limit but array size must be increased
    i++;
    floatNumber *= 10;       // for the next decimal
    temp = floatNumber;      // get the decimal
    ltoa(temp, str + ptr, 10);
    ptr++; digits++;         // Increment pointer and digits count
    floatNumber -= temp;     // Remove that digit
  }

  // Finally we can plot the string and return pixel length
  return print(str, scrX(poX), scrY(poY));
}

bool EzOledComp::btnClickOnRelease() {
  return _btnClickOnRelease;
}

void EzOledComp::btnClickOnRelease(bool clickOnRelease) {
  _btnClickOnRelease = clickOnRelease;
}

void EzOledComp::dbgOut() {
#if(EZ_DEBUG_ON)
  Serial.printf("\n\n----------------------------------------------------------\n");
  Serial.printf("EzOledComp::logProps()\n");
  Serial.printf("_text                = |%s|\n", _text != NULL ? _text : "NULL");
  Serial.printf("_btnClickOnRelease   = |%u|\n", _btnClickOnRelease);
  Serial.printf("_clickCancelsWindow  = |%u|\n", _clickCancelsWindow);
  Serial.printf("_inUse               = |%u|\n", _inUse);
  Serial.printf("_visible             = |%u|\n", _visible);
  Serial.printf("_enabled             = |%u|\n", _enabled);
  Serial.printf("_childCnt            = |%u|\n", _childCnt);
  Serial.printf("_poolPos             = |%u|\n", _poolPos);
  Serial.printf("_x                   = |%d|\n", _x);
  Serial.printf("_y                   = |%d|\n", _y);
  Serial.printf("_width               = |%d|\n", _width);
  Serial.printf("_height              = |%d|\n", _height);
  Serial.printf("_type                = |%d|\n", (int)_type);
  Serial.printf("_action              = |%d|\n", (int)_action);
  Serial.printf("_on                  = |%d|\n", (int)_on);
  Serial.printf("_font                = |%s|\n", _font == NULL ? F("NULL") : F("*HasVal"));
  Serial.printf("_scr                 = |%s|\n", _scr == NULL ? F("NULL") : F("*HasVal"));
  Serial.printf("_tft                 = |%s|\n", _tft == NULL ? F("NULL") : F("*HasVal"));
  Serial.printf("_wndPnl              = |%s|\n", _wndPnl == NULL ? F("NULL") : F("*HasVal"));
  Serial.printf("_parent              = |%s|\n", _parent == NULL ? F("NULL") : F("*HasVal"));
  Serial.printf("screenX()            = |%u|\n", screenX());
  Serial.printf("screenY()            = |%u|\n", screenY());
  Serial.printf("screenXRight()       = |%u|\n", screenXRight());
  Serial.printf("screenYBottom()      = |%u|\n", screenYBottom());
  Serial.printf("----------------------------------------------------------\n\n");
#endif
}

COMP_TYPE		EzOledComp::type() {
  return _type;
}

void EzOledComp::type(COMP_TYPE type) {
  _type = type;
}

EzOledComp* EzOledComp::parent() {
  return _parent;
}

uint16_t EzOledComp::leftPos() {
  return _x;
}

uint16_t EzOledComp::rightPos() {
  return _x + _width;
}

uint16_t EzOledComp::topPos() {
  return _y;
}

uint16_t EzOledComp::bottomPos() {
  return _y + _height;
}

COMP_ACT EzOledComp::action() {
  return _action;
}

uint16_t EzOledComp::x() {
  return _x;
}

uint16_t EzOledComp::y() {
  return _y;
}

uint16_t EzOledComp::screenX() {
  return scrX(0);
}

uint16_t EzOledComp::screenY() {
  return scrY(0);
}

uint16_t EzOledComp::screenXRight() {
  return scrX(0) + _width;
}

uint16_t EzOledComp::screenYBottom() {
  return scrY(0) + _height;
}

uint16_t EzOledComp::width() {
  return _width;
}

uint16_t EzOledComp::height() {
  return _height;
}

bool EzOledComp::inUse()	{
  return _inUse;
}

void EzOledComp::inUse(bool used) {
  if (used == false) {
    initialize();
  }
  _inUse = used;
}

ON_STATE EzOledComp::on() {
  return _on;
}

void EzOledComp::on(ON_STATE onState) {
  _on = onState;
}

void EzOledComp::addTask(Task* task) {
  _task = task;
}