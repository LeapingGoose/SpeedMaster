#include "main.h"

/**
 *  Core Routines
 */

void setup() {
#ifndef DISABLE_LOGGING
  ezlib::SerialBegin(115200);
#endif
  lg.begin(LOG_LVL, &Serial);
  initSPIFFS();
  initTasks();
  lg.notice(F("Starting Sensor Unit...\n"));
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    lg.error(F("Cannot mount SPIFFS volume.\n"));
  } else {
    lg.verbose(F("Mounted SPIFFS volume.\n"));
  }
}

void initTasks() {
  _srWiFi            .setWaiting();
  _srWiFiStation     .setWaiting();
  _srWiFiAccessPoint .setWaiting();
  _srUi              .setWaiting();

  /**      Task       Interval (MS)    Iterations    Callback       onEnable     onDisable      Trigger          Trigger Delay */
  initTask(_tPrefs,   TASK_IMMEDIATE,  TASK_ONCE,    &tPrefsInit,   NULL,        NULL);
  initTask(_tRestart, TASK_SECOND * 5, TASK_ONCE,    &tRestartInit, NULL,        NULL);
  initTask(_tUi,      TASK_IMMEDIATE,  TASK_ONCE,    &tUiInit,      NULL,        NULL,          &_srPrefsReady);
  initTask(_tNet,     TASK_IMMEDIATE,  TASK_ONCE,    &tNetInit,     NULL,        NULL,          &_srUi);
  initTask(_tBtn,     1,               TASK_FOREVER, &tBtnInit,     NULL,        NULL,          &_srPrefsReady);
  initTask(_tPnlDash, 100,             TASK_FOREVER, &tPnlDashCb,   NULL,        NULL,          &_srPrefsReady);
  initTask(_tTmr,     10,              TASK_FOREVER, &tTmrCb,       &tTmrEnable, &tTmrDisable);
  /** @todo Look into not refreshing _tPnlDash so often. Only refresh when panels require it. */

  _tPrefs.enable();
}

void loop() {
  _sched.execute();
}

/**
 *  Task: UI
 */

void tUiInit() {
  lg.verbose(F("tUiInit()\n"));
  _scr.setup();
  _scr.fontText(Dialog_plain_10);

  _scr.fontWndHdr(Dialog_plain_13);
  _scr.fontBtn(Dialog_plain_10);



  _scr.clearScreen();
  createBaseComp();
  showHdr("Go fast!");
  showMsg("Hold the big button...");
  _srUi.signalComplete();
}

void createBaseComp() {
  _scr.baseComp()->action(COMP_ACT::invisible);
  _pnlDash = createPnlDash();
  _scr.baseComp()->addChild(_pnlDash);
  _scr.baseComp()->action(COMP_ACT::visible);
}

EzOledComp* createPnlDash() {
  EzOledComp* pnlDash = _scr.createComp(0, 0, _scr.screenWidth(), _scr.screenHeight(), drawPnlDash, false);

  uint8_t y = 0;
  const uint8_t tmrPnlHeight = 31;
  const uint8_t hdrPnlHeight = 16;
  const uint8_t msgPnlHeight = 12;

  _pnlHdr = pnlDash->createChild(0, y, _scr.screenWidth(), hdrPnlHeight, drawPnlHdr, false);

  y += hdrPnlHeight + 2; //(_scr.screenHeight() / 2) - (tmrPnlHeight / 2);
  _pnlTimer = pnlDash->createChild(0, y, _scr.screenWidth(), tmrPnlHeight, drawPnlTimer, false);

  y += tmrPnlHeight + 2;
  _pnlMsg = pnlDash->createChild(0, y, _scr.screenWidth(), msgPnlHeight, drawPnlMsg, false);

  return pnlDash;
}

void drawPnlDash(EzOledComp* comp, COMP_ACT action) {
  // if (action & COMP_ACT_DRAW) {
  //   comp->drawOutline(1, 0, 0);
  //   comp->display();
  // }
}

void drawPnlHdr(EzOledComp* comp, COMP_ACT action) {
  if (action & COMP_ACT_DRAW) {
    comp->clear();
    // comp->setFont(ArialMT_Plain_16);
    comp->setFont(Dialog_plain_13);
    comp->print(_pnlHdrText, 0, 0);
    comp->display();
  }
}

void drawPnlTimer(EzOledComp* comp, COMP_ACT action) {
  if (action & COMP_ACT_DRAW) {
    comp->clear();
    // comp->setFont(ArialMT_Plain_24);
    comp->setFont(Timer_Font_26);
    comp->drawOutlineX(1, 0, 0, -1, 0, -1);
    comp->print(_sw.getFormatted(), 6, 0);
    comp->display();
  }
}

void drawPnlMsg(EzOledComp* comp, COMP_ACT action) {
  if (action & COMP_ACT_DRAW) {
    comp->clear();
    comp->setFont(Dialog_plain_10);
    comp->print(_pnlMsgText, 0, 0);
    comp->display();
  }
}

void showMsg(char* msg) {
  strcpy(&_pnlMsgText[0], msg);
  _pnlMsg->action(COMP_ACT::refresh);
}

void showHdr(char* msg) {
  strcpy(&_pnlHdrText[0], msg);
  _pnlHdr->action(COMP_ACT::refresh);
}

void tPnlDashCb() {
  _pnlDash->action(COMP_ACT::refresh, true);
}

/**
 * Task: Timer
 */

void tTmrCb() {
  // lg.verbose(F("tTmrCb()\n"));
  _pnlTimer->action(COMP_ACT::refresh);
}

bool tTmrEnable() {
  // lg.verbose(F("tTmrEnable()\n"));
  _sw.start();
  _isTmrRunning = true;
  return true;
}

void tTmrDisable() {
  // lg.verbose(F("tTmrDisable()\n"));
}

/**
 *  Task: Button
 */

void tBtnInit() {
  lg.verbose(F("tBtnInit()\n"));
  task_btn::setup(onBtnAction);
  _tBtn.setCallback(tBtnCb);
  _tBtn.forceNextIteration();
}

void tBtnCb() {
  task_btn::loop();
}

void onBtnAction(uint8_t btnId, BTN_EVENT action) {
  lg.verbose(F("btnId = %d / action = %d\n"), btnId, (uint8_t)action);

  // Button pressed with no timer running. Get ready to start the timer on release
  if (btnId == 1 && action == BTN_EVENT::RELEASE) {

    if (_isPrefsShowing) {
      _scr.closeWindow();
      _isPrefsShowing = false;
    } else {
      _scr.showWindow(createWindowPanelPrefs);
      _isPrefsShowing = true;
    }

  } else if (_resetTimer && btnId == 2 && action == BTN_EVENT::PRESS) {
    _resetTimer = false;
    _sw.reset();
    _pnlTimer->action(COMP_ACT::refresh);
    showHdr("Give it your all!");
    showMsg("Hold the big button...");

  } else if (_isTmrRunning == false && btnId == 2 && action == BTN_EVENT::PRESS) {
    showHdr("Ready...");
    showMsg("Let go to start.");
    _startTimerOnRelease = true;

  // Buton pressed with timer running. Stop the timer.
  } else if (_isTmrRunning == true && btnId == 2 && action == BTN_EVENT::PRESS) {
    lg.verbose(F("STOP TIMER\n"));
    _sw.stop();
    _isTmrRunning = false;
    showHdr("Done!");
    showMsg("Press again to reset...");

    _tTmr.disable();
    _startTimerOnRelease = false;
    _resetTimer = true;

  // Button released and ready for timer. Start timing.
  } else if (_startTimerOnRelease && btnId == 2 && action == BTN_EVENT::RELEASE) {
    lg.verbose(F("START TIMER\n"));
    _tTmr.enable();
    showHdr("GO GO GO!!!");
    showMsg("Press again to stop.");

  } else {
    lg.verbose(F("DO NOTHING\n"));
  }

}

/**
 *  Task: Preferences (like EEPROM)
 */

void tPrefsInit() {
  lg.verbose(F("tPrefsInit()\n"));
  task_prefs::serialOut();
  _srPrefsReady.signalComplete();
}

EzOledComp* createWindowPanelPrefs() {
  // EzOledComp* comp = _scr.createWindowPanel(drawPnlPrefs, dict(DICT::prefs));
  EzOledComp* comp = _scr.createWindowPanel(drawPnlPrefs, "Merry Christmas!");
  return comp;
}

void drawPnlPrefs(EzOledComp* comp, COMP_ACT action) {

  if (action & (COMP_ACT_DRAW | COMP_ACT_RELEASE)) {
    comp->clear();
    comp->setFont(Dialog_plain_10);
    // comp->print(dict(DICT::email), 0, 0);
    comp->print("I love you Cooper,", 0, 5);
    comp->print("Daddy", 0, 18);
    comp->print("XOxo", 0, 31);
    comp->display();
  }
}

/**
 *  Task: Restart
 *  Restarts the microprocessor but allows time to display information
 *  to the user before restarting.
 */
void tRestartInit() {
  lg.verbose(F("tRestartInit()\n"));
  ESP.restart();
}

void restart(const char* msg) {
  lg.verbose(F("restart(...)\n"));

  if (msg != NULL) {
    lg.notice(F("%s \n"), msg);
  }

  _tRestart.enableDelayed(TASK_SECOND * 5);
}

/**
 *  Task: Network
 */

void tNetInit() {
  lg.verbose(F("tNetInit()\n"));
  task_net::setup(onNetStatusUpdateCb, onSocketInCb, false);
}

/** Don't need these other methods for the network */
void tNetDisable() {
  lg.verbose(F("tNetDisable()\n"));
}

void onNetStatusUpdateCb(NET_JOB jobType, NET_JOB_STATUS jobStatus) {
  using namespace task_net;
  lg.verbose(F("main::onNetStatusUpdateCb(...)\n"));

  switch (jobType)
  {
  case NET_JOB::STATION:      onStationStatusUpdate(jobStatus);     break;
  case NET_JOB::ACCESS_POINT: onAccessPointStatusUpdate(jobStatus); break;
  case NET_JOB::OTA:          onOtaStatusUpdate(jobStatus);         break;
  case NET_JOB::REBOOT:       restart("Restarting...");             break;
  case NET_JOB::UNKNOWN:
  default:                                                          break;
  }
}

void onStationStatusUpdate(NET_JOB_STATUS jobStatus) {
  lg.verbose(F("main::onStationStatusUpdate(...)\n"));

  if (jobStatus == NET_JOB_STATUS::START) {
    _srWiFi.signalComplete();

  } else if (jobStatus == NET_JOB_STATUS::CONNECTED) {
    lg.notice(F("STA CONNECTED\n"));
    _srWiFiStation.signalComplete();
    _srWiFi.signalComplete();

  } else {
    lg.notice(F("STA DISCONNECTED\n"));
    _srWiFiStation.signalComplete(-1);
    _srWiFi.signalComplete(-1);
  }
}

void onAccessPointStatusUpdate(NET_JOB_STATUS jobStatus) {
  lg.verbose(F("main::onAccessPointStatusUpdate(...)\n"));

  if (jobStatus == NET_JOB_STATUS::CONNECTED) {
    lg.notice(F("AP CONNECTED\n"));
    _srWiFiAccessPoint.signalComplete();
    _srWiFi.signalComplete();

  } else {
    lg.notice(F("AP DISCONNECTED\n"));
    _srWiFiAccessPoint.signalComplete(-1);
    _srWiFi.signalComplete(-1);
  }
}

void onOtaStatusUpdate(NET_JOB_STATUS jobStatus) {
  lg.verbose(F("main::onOtaStatusUpdate(...)\n"));
}

/**
 * Web Sockets
 */
// If 'key' is NULL, there was only a single value sent.
void onSocketInCb(AsyncWebSocketClient* client, AwsEventType type, const char* data) {
  lg.verbose(F("onSocketInCb(...)\n"));

  switch (type)
  {
  case AwsEventType::WS_EVT_CONNECT:
    lg.verbose(F("onSocketInCb(...) type = WS_EVT_CONNECT\n"));
    break;
  
  case AwsEventType::WS_EVT_DATA:
    lg.verbose(F("onSocketInCb(...) type = WS_EVT_DATA\n"));
    break;
  
  case AwsEventType::WS_EVT_DISCONNECT:
    lg.verbose(F("onSocketInCb(...) type = WS_EVT_DISCONNECT\n"));
    break;
  
  case AwsEventType::WS_EVT_ERROR:
    lg.verbose(F("onSocketInCb(...) type = WS_EVT_ERROR\n"));
    break;
  
  case AwsEventType::WS_EVT_PONG:
    lg.verbose(F("onSocketInCb(...) type = WS_EVT_PONG\n"));
    break;
  }
}

/**
 * Sensors
 */
void tSendReadingsCb() {
  lg.verbose(F("main::tSendReadingsCb(...)\n"));
}

void startTimer() {
  lg.verbose(F("startTimer()\n"));
  _sw.start();
}

void stopTimer() {
  lg.verbose(F("stopTimer() %F\n"), _sw.getElapsedAs100th());
  _sw.stop();
}