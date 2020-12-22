#include "EzOled.h"

/**************************************************************************************************
*		EZTouchForms Class
**/

EzOled::EzOled() {
}

EzOled::~EzOled() {
  // delete _ts;
}

void EzOled::setup(byte orientation) {
  _tft->init();
  _tft->flipScreenVertically();
  _tft->setTextAlignment(TEXT_ALIGN_LEFT);
  _tft->setFont(ArialMT_Plain_10);
  _tft->clear();

  _orientation = orientation;
  _baseComp = createComp(0, 0, screenWidth()-1, screenHeight()-1, NULL, false);
  _currBaseComp = _baseComp;
}

// @todo Clean up all the repetition in the contructors (here and everywhere).
// @todo Should have the option to pass the parent to all constructuors. Without that, you can't reliable set isVisible to true here since it won't have parents coordinates yet.
EzOledComp* EzOled::createComp(EzOledComp* parent, int16_t x, int16_t y, int16_t width, int16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible) {
  EzOledComp* comp = getCompFromPool();
  comp->setup(this, parent, x, y, width, height, drawFunc, isVisible);
  return comp;
}

EzOledComp* EzOled::createComp(int16_t x, int16_t y, int16_t width,int16_t height, void (*drawFunc)(EzOledComp* comp, COMP_ACT action), bool isVisible) {
  EzOledComp* comp = getCompFromPool();
  comp->setup(this, x, y, width, height, drawFunc, isVisible);
  return comp;
}

EzOledComp* EzOled::createComp(COMP_TYPE type, int16_t x, int16_t y, int16_t width,int16_t height, void (*drawFunc)( EzOledComp* comp, COMP_ACT action), bool isVisible) {
  EzOledComp* comp = getCompFromPool();
  comp->setup(this, x, y, width, height, drawFunc, isVisible);
  comp->type(type);
  return comp;
}

EzOledComp* EzOled::createWindowPanel(void (*drawFunc)(EzOledComp* comp, COMP_ACT action), const char* header) {
  EzOledComp* comp = getCompFromPool();
  comp->setup(this, drawFunc);
  comp->text(header);
  _newHeaderText = comp->text();
  return comp;
}

void EzOled::loop() {
}

EzOledComp* EzOled::getCompFromPool() {
  EzOledComp* nextAvailabeCompInPool = NULL;
  if (_compsInUse + 1 >= EZTF_MAX_COMPONENTS) {
    Serial.println(); Serial.println(); Serial.print(F("CRITICAL ERROR: EZTF_MAX_COMPONENTS exceeded. Failure imminent."));
    // Ensure we have time to write the error. It doesn't matter what the delay is
    // since failure is on the way!
    delay(5000);
    return NULL;
  }
  if (++_compPoolPos > EZTF_MAX_COMPONENTS) {
    _compPoolPos = 0;
  }

  for (uint8_t n = 0; n < EZTF_MAX_COMPONENTS; n++) {
    if (_compPool[n].inUse() == false) {
      nextAvailabeCompInPool = &(_compPool[n]);
      _compsInUse++;
#if(EZ_DEBUG_ON)
      Serial.printf("_compsInUse +: %u of %d\n", _compsInUse, EZTF_MAX_COMPONENTS);
#endif
      nextAvailabeCompInPool->poolPos(n);
      nextAvailabeCompInPool->inUse(true);
      break;
    }
  }

  return nextAvailabeCompInPool;
}

void EzOled::returnToPool(EzOledComp* comp) {
  _compsInUse--;
#if(EZ_DEBUG_ON)
  Serial.printf("_compsInUse -: %u of %d\n", _compsInUse, EZTF_MAX_COMPONENTS);
#endif
  comp->inUse(false);
}

// comp is the callers "canvas" component where they draw what they
// want in their window. Everything else in the window is automatcally
// handled (close buton, header, etc...)
void EzOled::showWindow(EzOledComp* (*createWindowPanelFunc)(), bool useCancelBtn) {
  // Remove the previous window, if there was one.
  if (_ezWindowsCnt > 0) {
    // Reset the current window to free up memory so we can create the next window.
    _wndStack[_ezWindowsCnt-1].component->action(COMP_ACT::reset);
  } else {
    _baseComp->action(COMP_ACT::invisible);
  }

  _ezWindowsCnt++;

  _wndStack[_ezWindowsCnt-1].component = createEmptyWindow(useCancelBtn);
  _wndStack[_ezWindowsCnt-1].createWindowPanelFunc = createWindowPanelFunc;

  // Create the panel that will be used in the content area of the window.
  EzOledComp* wndPnl = _wndStack[_ezWindowsCnt-1].createWindowPanelFunc();
  _wndStack[_ezWindowsCnt-1].header = wndPnl->text();

  // Set the header text to the header component for later rendering.
  // _wndStack[_ezWindowsCnt-1].component->getChild(0)->text(wndPnl->text());
  _currWindow = _wndStack[_ezWindowsCnt-1].component;
  _currBaseComp = _currWindow;
  _currHeader = _wndStack[_ezWindowsCnt-1].header;
  

  // We'll hold on to this here so we can call it's callback on window close.
  _currWindow->windowPanel(wndPnl);
  _currWindow->action(COMP_ACT::visible);
}

void EzOled::closeWindow(bool cancel) {
  // @todo May consider removing this and letting the user handle filling when painting new components.
  _currWindow->action(cancel ? COMP_ACT::cancel : COMP_ACT::close_wnd);
  
  // We're now sitting on the previous window and will recreate it, if there is one.
  _ezWindowsCnt--;
  _wndStack[_ezWindowsCnt].createWindowPanelFunc = NULL;
  _wndStack[_ezWindowsCnt].component = NULL;

  // @todo This code is repeated in showWindow().
  if (_ezWindowsCnt > 0) {
    // Recreate the previous window from scratch
    _wndStack[_ezWindowsCnt-1].component = createEmptyWindow();
    _currWindow = _wndStack[_ezWindowsCnt-1].component;
    EzOledComp* wndPnl = _wndStack[_ezWindowsCnt-1].createWindowPanelFunc();
    _currBaseComp = _currWindow;
    _currHeader = _wndStack[_ezWindowsCnt-1].header;
    _currWindow->windowPanel(wndPnl);
    // @todo Perhaps visible is what we should use here.
    _currWindow->action(COMP_ACT::enabled);
  } else {
    _currHeader = NULL;
    _currWindow = NULL;
    _currBaseComp = _baseComp;
    // Clear it out before we redraw it since we have the old window contents showing right now.

    // _baseComp->fill(__thm.color.bg);
    _tft->clear();
    _baseComp->action(COMP_ACT::visible);
  }
}

EzOledComp* EzOled::createEmptyWindow(bool useCancelBtn) {
  // Draw the generic parts of the window

  if (_ezWindowsCnt > DISP_MAX_WINDOWS) {
    // @todo This should never happen, but add code to anticipate a null return;
    return NULL;
  }

  // Draw the window form
  EzOledComp* frm = createComp(COMP_TYPE::window, 0, 0, screenWidth(), screenHeight(), NULL);

  // Draw the window header bar
  EzOledComp* hdr = frm->createChild(COMP_TYPE::window_header, 0, 0, frm->width(), DISP_WND_HDR_HEIGHT, NULL);
    
  hdr->setFont(_fontWndHdr);
  // hdr->print(_newHeaderText);

  // Serial.print("_currHeader = ");
  // Serial.println(_currHeader);
  // hdr->print(_currHeader, 0, 0);

  // Draw the window header back button
  // hdr->createChild(COMP_TYPE::window_back_btn, 0, 0, DISP_BTN_CLOSE_WIDTH, DISP_BTN_CLOSE_HEIGHT, NULL);

  // if (useCancelBtn) {
  //   // Draw the window header cancel button
  //   hdr->createChild(COMP_TYPE::window_cancel_btn, screenWidth()-3-DISP_BTN_CANCEL_WIDTH, 0, DISP_BTN_CANCEL_WIDTH, DISP_BTN_CANCEL_HEIGHT, NULL);
  // }

  // Draw the window header text
  // hdr->createChild(COMP_TYPE::window_header_text, DISP_BTN_CLOSE_WIDTH + DISP_WND_HDR_CLOSE_BTN_SPACING, 0, frm->width()-DISP_BTN_CLOSE_WIDTH-DISP_WND_HDR_CLOSE_BTN_SPACING, DISP_BTN_CLOSE_HEIGHT, NULL);
  // hdr->createChild(COMP_TYPE::window_header_text, 0, 0, frm->width(), DISP_WND_HDR_HEIGHT, NULL);
  return frm;
}

SSD1306Wire* EzOled::tft() {
  return _tft;
}

void EzOled::reverseX() {
  swapVals(int16_t ,_minX, _maxX);
}

void EzOled::reverseY() {
  swapVals(int16_t ,_minY, _maxY);
}

int16_t EzOled::screenWidth() {
  // Should get it from the _tft object but the width from it was acting up. Just getting past it with this.
  // @todo Use _tft->width();
  // return TS_MAX_X + 1;
  return TS_MAX_X;
}

int16_t EzOled::screenHeight() {
  return TS_MAX_Y;
}

void EzOled::closeBtnIcon(const uint8_t* icon) {
  _closeBtnIcon = icon;
}

const uint8_t* EzOled::closeBtnIcon() {
  return _closeBtnIcon;
}

void EzOled::cancelBtnIcon(const uint8_t* icon) {
  _cancelBtnIcon = icon;
}

const uint8_t* EzOled::cancelBtnIcon() {
  return _cancelBtnIcon;
}

void EzOled::setFont(const uint8_t* font) {
  _tft->setFont(font);
}

void EzOled::fontWndHdr(const uint8_t font[]) {
  if (font == nullptr) return;
  _fontWndHdr = (uint8_t*) font;
}

const uint8_t* EzOled::fontWndHdr() {
  return _fontWndHdr;
}

void EzOled::fontBtn(const uint8_t font[]) {
  _fontBtn = font;
}

const uint8_t* EzOled::fontBtn() {
  return _fontBtn;
}

// void EzOled::setTextColor(uint16_t fg, uint16_t bg) {
//   _tft->text
//   _tft->setTextColor(fg, bg);
// }

void EzOled::baseComp(EzOledComp* baseComp) {
  _baseComp = baseComp;
}

EzOledComp* EzOled::baseComp() {
  return _baseComp;
}

const char * EzOled::currWindowHdr() {
  return _currHeader;
}

const char * EzOled::newWindowHdr() {
  // LEFT OFF HERE>> Need to get the header showing.
  return _newHeaderText;
}

EzOledComp* EzOled::currWindow() {
  return _currWindow;
}

EzOledComp* EzOled::currComp() {
  return _currBaseComp;
}

void EzOled::fontText(const uint8_t font[]) {
  _fontText = font;
}

const uint8_t* EzOled::fontText() {
  return _fontText;
}

void EzOled::clearScreen() {
  Serial.printf("_tft == NULL = %s\n", _tft == NULL ? "true" : "false");
  _tft->clear();
}