#ifndef TASK_NETWORK_H
#define TASK_NETWORK_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <IPAddress.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <EzPzTimer.h>
#include <common-network.h>
#include <WiFi.h>
#include <stdint.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>

enum ParseResult: uint8_t {
  PARSED_OK = 0,
  PARSE_FAILED,
  NOTHING_CHANGED,
};

/**
 *  This network task takes care of WiFi, Mesh and Web Server.
 */
// typedef void (*onSocketIn) (AwsEventType type, AppState& appState);
typedef void (*onSocketInFn) (AsyncWebSocketClient* client, AwsEventType type, const char* data);
typedef String (*pageProcessorFn) (const String& var);


#define FIRMWARE_VERSION "v0.0.1"

namespace task_net {

  #define TMR_MS_ESPNOW           100   // Let the ESP-Now library process raw in/out messages.
  #define TMR_MS_PROCESS_MSG_BUFF 200   // Processes any incoming/outgoing messages in our buffers filled by ESP-Now.
  #define TMR_MS_WIFI_CHECK_SLOW  10000 // Used to idly check for a wifi connection.
  #define TMR_MS_WIFI_CHECK_FAST  100   // USed when quickly checking for a wifi connection.

  /**
   * General
   ***********************************************************************************************/
  void           setup(onNetStatusUpdateFn onNetStatusUpdateFn, onSocketInFn onWsDataInCb, bool hideServer);
  bool           isInitialized();
  void           reset();

  /**
   * MAC Address
   */
  String         getMacAddrStr();
  uint8_t*       getMacAddr();
  void           getMacAddr(uint8_t* macAddr);

  /**
   * WiFi
  */
  bool           isWifiConnected();
  bool           isWiFiObtainingIp();
  void           saveIp(IPAddress ipAddr);
  IPAddress      getStationIp();
  IPAddress      getAccessPointIp();
  int32_t        getWifiChannel();
  void           setApSsid(char* ssid);
  char*          getApSsid();
  void           setApPwd(char* pwd);
  char*          getApPwd();

  /**
   * Website
   */
  void           setWebsiteUsername(char* username);
  char*          getWebsiteUsername();
  void           setWebsitePassword(char* password);
  char*          getWebsitePassword();
  void           setWebsiteUrl(char* url);
  char*          getWebsiteUrl();

  /**
   * Sockets
   */
  void           sendSocketMsg(char* msg, bool forceAll = false);
  void           sendSocketMsg(uint32_t clientId, char* msg, bool forceAll);

}

#endif