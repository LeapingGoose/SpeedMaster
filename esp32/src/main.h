#ifndef GROW_MAIN_H
#define GROW_MAIN_H

#include <Arduino.h>
// _TASK_STATUS_REQUEST should be defined but it's causing core panics.
// I was able to get the desired functionality with no issues
// by manually commenting out the #ifndef _TASK_STATUS_REQUEST in
// the TaskScheduler library (not good) and it seems to work. The issue would
// appear to be something to do with the compiler getting mixed
// messages about what _TASK_STATUS_REQUEST is.
#define _TASK_STATUS_REQUEST 
#include <TaskScheduler.h>
#include <stdint.h>
#include <EzOledShared.h>
#include <EzOled.h>
#include <EzOledComp.h>
#include <EzLib.h>
#include <flint.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include <common-network.h>
#include <common-sensors.h>
#include <TaskScheduler.h>
#include "task_button.h"
#include "task_prefs.h"
#include <task_network.h>
#include <task_ui.h>
#include <TaskSchedulerHelper.h>
#include <Stopwatch.h>
#include <ExtraFonts.h>
#include <dialog_13.h>
#include <dialog_10.h>
#include "AppDict.h"

using namespace app_dict;

/**
 *  Prototypes
 */

/** General */
void initTasks();
void initSPIFFS();
void tPnlDashCb();

EzOledComp* createWindowPanelPrefs();
void drawPnlPrefs(EzOledComp* comp, COMP_ACT action);


/** UI */
void tUiInit();

/** Button */
void tBtnInit();
void tBtnCb();
void onBtnAction(uint8_t btnId, BTN_EVENT action);

/** Preferences (in place of EEPROM) */
void tPrefsInit();

/** Network */
void tNetInit();
void tNetDisable();
void setWiFiTasksToWait();
void onStationStatusUpdate(NET_JOB_STATUS jobStatus);
void onAccessPointStatusUpdate(NET_JOB_STATUS jobStatus);
void onOtaStatusUpdate(NET_JOB_STATUS jobStatus);
void onNetStatusUpdateCb(NET_JOB jobType, NET_JOB_STATUS netStatus);
void onSocketInCb(AsyncWebSocketClient* client, AwsEventType type, const char* data);
void tSendReadingsCb();

/** Bounce - Restarts the server */
void tRestartInit();
void restart(const char* shortMsg, const char* longMsg);

/** Timer */
void startTimer();
void stopTimer();
void tTmrCb();
bool tTmrEnable();
void tTmrDisable();

/**
 *  Variables / Constants
 */

/** UI */
#define COMP_ACT_RELEASE     (COMP_ACT::releasedSoft | COMP_ACT::releasedHard | COMP_ACT::unhold)
#define COMP_ACT_DRAW        (COMP_ACT::initialize | COMP_ACT::enabled | COMP_ACT::visible | COMP_ACT::refresh)
#define COMP_ACT_BTN_RELATED (COMP_ACT_RELEASE | COMP_ACT::visible | COMP_ACT::justPressed)
#define COMP_CLICK           (COMP_ACT::releasedHard | COMP_ACT::unhold)
#define COMP_HOLD            (COMP_ACT::holding)
#define COMP_UNHOLD          (COMP_ACT::unhold)

EzOled _scr;
// Components
EzOledComp* _pnlDash        = NULL;
EzOledComp* _pnlApIp        = NULL;
EzOledComp* _pnlTimer       = NULL;
EzOledComp* _pnlMsg         = NULL;
EzOledComp* _pnlWifiChannel = NULL;
EzOledComp* _pnlHdr         = NULL;
char _pnlMsgText[40] = "";
char _pnlHdrText[40] = "";

#define TIMER_FONT_HEIGHT 26

void createBaseComp();
EzOledComp* createPnlDash();
void drawPnlDash(EzOledComp* comp, COMP_ACT action);
void drawPnlTimer(EzOledComp* comp, COMP_ACT action);
void drawPnlGeneral(EzOledComp* comp, COMP_ACT action);
void drawPnlHdr(EzOledComp* comp, COMP_ACT action);
void drawPnlMsg(EzOledComp* comp, COMP_ACT action);
void showMsg(char* msg);
void showHdr(char* msg);

/**
 * Log Levels for 'lg.<...>' calls throughout the system.
 * @note For production, turn off logging completely by
 * uncommenting line 31 (definition of DISABLE_LOGGING) in
 * ArduinoLog.h
*/
// #define LOG_LVL LOG_LEVEL_SILENT
// #define LOG_LVL LOG_LEVEL_FATAL
// #define LOG_LVL LOG_LEVEL_ERROR
// #define LOG_LVL LOG_LEVEL_WARNING
// #define LOG_LVL LOG_LEVEL_NOTICE
// #define LOG_LVL LOG_LEVEL_TRACE
#define LOG_LVL LOG_LEVEL_VERBOSE

/** UI */
bool _isPrefsShowing = false;

/** Task Schedulers **/
Scheduler     _sched;
StatusRequest _srUi;
StatusRequest _srWiFi;
StatusRequest _srPrefsReady;
StatusRequest _srWiFiStation;
StatusRequest _srWiFiAccessPoint;

/** Timer */
Stopwatch _sw;
bool _isTmrRunning = false;
bool _startTimerOnRelease = false;
bool _resetTimer = false;

/** Tasks **/
Task _tPrefs        (NULL, &_sched);
Task _tRestart      (NULL, &_sched);
Task _tUi           (NULL, &_sched);
Task _tBtn          (NULL, &_sched);
Task _tNet          (NULL, &_sched);
Task _tPnlDash      (NULL, &_sched);
Task _tTmr          (NULL, &_sched);

#endif