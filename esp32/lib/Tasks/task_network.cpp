#include "task_network.h"
#include "webpages.h"

// LEFT OFF HERE> Get AP working with pretty URL
// LEFT OFF HERE> Get AP working without STA connecting.

namespace task_net {

  namespace {
 
    #define MAX_LEN_SSID_NAME 32
    #define MAX_LEN_SSID_PWD 64
    #define MAX_LEN_WEB_APP_URL 100

    /**
    *  Private
    ******************************************************************************************/
    /** Task */
    bool                   _isInitialized = false;

    /** Web Server */
    AsyncWebServer         _server(80);
    AsyncWebSocket         _ws("/ws");
    String                 _stateAsTxt;

    /** WiFi */
    bool                   _apStarted = false;
    WIFI_SERVER_TYPE       _srvType = WIFI_SERVER_TYPE::AP_STA;
    char                   _ipAddrTxt[16];
    IPAddress              _ipAddr;
    uint8_t                _macAddr[6] = { 0, 0, 0, 0, 0, 0 };
    EzPzTimer              _tmrWifiCheck;
    onNetStatusUpdateFn    _onNetStatusUpdateCb;
    onSocketInFn           _onSocketInCb;
    bool                   _hideApServer;
    char                   _sckMsgBuff[SCK_MSG_SIZE];
    char                   _websiteUsername[20] = "gofast";
    char                   _websitePassword[20] = "faster";
    char                   _apSsid[MAX_LEN_SSID_NAME] = DEFAULT_BASE_AP_SSID;
    char                   _apPwd[MAX_LEN_SSID_PWD] = DEFAULT_BASE_AP_PASSWORD;
    char                   _webAppUrl[MAX_LEN_WEB_APP_URL] = DEFAULT_WEB_DOMAIN;

    IPAddress staticIp(0, 0, 0, 0);
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns;
    IPAddress emptyIp(0, 0, 0, 0);

    /** OTA Updating */
    NET_JOB_STATUS _otaState = NET_JOB_STATUS::UNKNOWN;

    /** Prototypes */
    void        onWifiEvent(WiFiEvent_t event);
    void        setupWebServer();
    void        wifiConnectSta();
    void        startAsStandardBaseUnit();
    void        initWebSocket();
    void        onSocketIn(AsyncWebSocket* server, AsyncWebSocketClient* client,  AwsEventType type, void* arg, uint8_t* data, size_t len);
    ParseResult parseSocketDataIn(AsyncWebSocketClient* client, char* strMsg, size_t len);
    String      humanReadableSize(const size_t bytes);
    String      pageProcessor(const String& var);


    /** Private Functions */
    void onWifiEvent(WiFiEvent_t event) {

      switch(event)
      {
      case SYSTEM_EVENT_STA_START:
        lg.verbose(F("task_network:: SYSTEM_EVENT_STA_START" CR));
        /** @todo not an accurate state but works for now **/
        _onNetStatusUpdateCb(NET_JOB::STATION, NET_JOB_STATUS::CONNECTED);
        break;

      case SYSTEM_EVENT_STA_DISCONNECTED:         /**< ESP32 station disconnected from AP */
        lg.verbose(F("task_network:: SYSTEM_EVENT_STA_DISCONNECTED" CR));
        _onNetStatusUpdateCb(NET_JOB::STATION, NET_JOB_STATUS::DISCONNECTED);
        break;

      case SYSTEM_EVENT_STA_GOT_IP: {              /**< ESP32 station got IP from connected AP */
        lg.verbose(F("task_network:: SYSTEM_EVENT_STA_GOT_IP" CR));
        lg.notice(F("Station IP: %s" CR), WiFi.localIP().toString().c_str());

        // // True if we're connecting for the first time in which case we
        // // got a dynamic IP address. Next we'll reconnect to that address
        // // statically.
        // if (staticIp == IPAddress(0, 0, 0, 0)) {
        //   lg.verbose(F("task_net:: Dynamic Connection.\n"));
        //   staticIp = WiFi.localIP();
        //   gateway = WiFi.gatewayIP();
        //   subnet = WiFi.subnetMask();
        //   dns = WiFi.dnsIP(0);
        //   // Reconnect statically with the IP address we were assigned.
        //   wifiConnectSta();
        // } else {
        //   lg.verbose(F("task_net:: Static Connection.\n"));
        // }

        lg.trace(F("\n\ntask_network:: \\o/ WiFi Station Connected!" CR));
        lg.trace(F("[WIFI] Local IP....: %s\n"), staticIp.toString().c_str());
        lg.trace(F("[WIFI] Subnet Mark.: %s\n"), subnet.toString().c_str());
        lg.trace(F("[WIFI] Gateway IP..: %s\n"), gateway.toString().c_str());
        lg.trace(F("[WIFI] DNS 1.......: %s\n"), dns.toString().c_str());
        lg.trace(F("[WIFI] DNS 2.......: %s\n"), WiFi.dnsIP(1).toString().c_str());
        lg.trace(F("[WIFI] SSID........: %s\n"), WiFi.SSID().c_str());
        lg.trace(F("[WIFI] BSSID.......: %s\n"), WiFi.BSSIDstr().c_str());
        lg.trace(F("[WIFI] MAC.........: %s\n"), WiFi.macAddress().c_str());
        lg.trace(F("[WIFI] Channel.....: %d\n\n"), WiFi.channel());

        _onNetStatusUpdateCb(NET_JOB::STATION, NET_JOB_STATUS::CONNECTED);

        break;
      }
      case SYSTEM_EVENT_STA_LOST_IP:              /**< ESP32 station lost IP and the IP is reset to 0 */
        lg.verbose(F("task_network:: SYSTEM_EVENT_STA_LOST_IP" CR));        // _obtainingIp = true;
        _onNetStatusUpdateCb(NET_JOB::STATION, NET_JOB_STATUS::DISCONNECTED);
        break;

      case SYSTEM_EVENT_AP_START:                 /**< ESP32 soft-AP start */
        if (_apStarted != true) {
          _apStarted = true;
          lg.verbose(F("task_network:: SYSTEM_EVENT_AP_START" CR));
          lg.notice(F("task_network:: Access Point IP: %s" CR), WiFi.softAPIP().toString().c_str());
          _onNetStatusUpdateCb(NET_JOB::ACCESS_POINT, NET_JOB_STATUS::CONNECTED);
          lg.notice(F("This Unit's MAC: %s" CR), WiFi.macAddress().c_str());
        }
        break;

      case SYSTEM_EVENT_AP_STOP:                  /**< ESP32 soft-AP stop */
        lg.verbose(F("task_network:: SYSTEM_EVENT_AP_STOP" CR));
        _apStarted = false;
        _onNetStatusUpdateCb(NET_JOB::ACCESS_POINT, NET_JOB_STATUS::DISCONNECTED);
        break;

      default:
        break;
      }
    }

    bool checkUserWebAuth(AsyncWebServerRequest * request) {
      bool isAuthenticated = false;

      if (request->authenticate(_websiteUsername, _websitePassword)) {
        Serial.println("is authenticated via username and password");
        isAuthenticated = true;
      }
      return isAuthenticated;
    }

    String listFiles(bool ishtml) {
      String returnText = "";
      Serial.println("Listing files stored on SPIFFS");
      File root = SPIFFS.open("/");
      File foundfile = root.openNextFile();

      if (ishtml) {
        returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
      }

      while (foundfile) {

        if (ishtml) {
          returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td>";
          // returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'download\')\">Download</button>";
          returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";

        } else {
          returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
        }

        foundfile = root.openNextFile();
      }

      if (ishtml) {
        returnText += "</table>";
      }

      root.close();
      foundfile.close();
      return returnText;
    }

    String humanReadableSize(const size_t bytes) {
      if (bytes < 1024) return String(bytes) + " B";
      else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
      else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
      else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
    }

    String pageProcessor(const String& var) {
      if (var == "FIRMWARE") {
        return FIRMWARE_VERSION;
      }

      if (var == "FREESPIFFS") {
        return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
      }

      if (var == "USEDSPIFFS") {
        return humanReadableSize(SPIFFS.usedBytes());
      }

      if (var == "TOTALSPIFFS") {
        return humanReadableSize(SPIFFS.totalBytes());
      }

      if (var == "IP_AP") {
        return getAccessPointIp().toString();
      }

      if (var == "IP_STA") {
        return getStationIp().toString();
      }
      return String();
    }

    void setupWebServer() {
      lg.verbose(F("task_network::setupWebServer()\n\n"));

      if (_srvType == WIFI_SERVER_TYPE::UNKNOWN) {
        // Removing web server.
        _server.end();
        lg.verbose(F("task_network:: Disabled Web Server." CR));
      } else {

        /** HTTP_GET: '/'
        ******************************************************************************************/
        /** a.bin == index.html */
        

        // presents a "you are now logged out webpage
        _server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
          String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
          Serial.println(logmessage);
          request->send_P(401, "text/html", logout_html);
        });

        // // Download file
        _server.on("/file", HTTP_GET, [](AsyncWebServerRequest * request) {
          lg.verbose(F("task_net::HTTP_GET: '/%s'\n"), request->url());

          if (checkUserWebAuth(request)) {

            if (request->hasParam("name") && request->hasParam("action")) {
              const char *fileName = request->getParam("name")->value().c_str();
              const char *fileAction = request->getParam("action")->value().c_str();

              if (!SPIFFS.exists(fileName)) {
                lg.verbose(F("ERROR: SPIFFS file requested does not exist.\n"));
                request->send(400, "text/plain", "ERROR: file does not exist");
              } else {

                if (strcmp(fileAction, "download") == 0) {
                  request->send(SPIFFS, fileName, "application/octet-stream");

                } else if (strcmp(fileAction, "delete") == 0) {
                  SPIFFS.remove(fileName);
                  request->send(200, "text/plain", "Deleted File: " + String(fileName));

                } else {
                  request->send(400, "text/plain", "ERROR: invalid action param supplied");
                }
              }
            } else {
              request->send(400, "text/plain", "ERROR: name and action params required");
            }
          } else {
            return request->requestAuthentication();
          }
        });

        _server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
          String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();

          if (checkUserWebAuth(request)) {
            request->send(200, "text/html", reboot_html);
            _onNetStatusUpdateCb(NET_JOB::REBOOT, NET_JOB_STATUS::UNKNOWN);
          } else {
            return request->requestAuthentication();
          }
        });

        _server.on("/settings", HTTP_GET, [](AsyncWebServerRequest * request) {
          lg.verbose(F("task_net::HTTP_GET: '/%s'\n"), request->url());

          // if (checkUserWebAuth(request)) {
            request->send_P(200, "text/html", _otaSpiffsIndexHtml, pageProcessor);
          // } else {
          //   return request->requestAuthentication();
          // }
        });

        _server.on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
        {
          String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
          if (checkUserWebAuth(request)) {
            logmessage += " Auth: Success";
            Serial.println(logmessage);
            request->send(200, "text/plain", listFiles(true));
          } else {
            logmessage += " Auth: Failed";
            Serial.println(logmessage);
            return request->requestAuthentication();
          }
        });

        _server.onFileUpload([](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
          lg.verbose(F("task_net::onFileUpload(...)\n"));
          // make sure authenticated before allowing upload
          if (checkUserWebAuth(request)) {
            String logmessage; // = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
            // Serial.println(logmessage);

            if (!index) {
              // logmessage = "Upload Start: " + String(filename);
              // open the file on first call and store the file handle in the request object
              request->_tempFile = SPIFFS.open("/" + filename, "w");
              Serial.printf("Upload Start: %s\n", filename.c_str());
            }

            if (len) {
              // stream the incoming chunk to the opened file
              request->_tempFile.write(data, len);
              logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
              Serial.println(logmessage);
            }

            if (final) {
              logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
              // close the file handle as the upload is now done
              request->_tempFile.close();
              Serial.println(logmessage);
              request->redirect("/");
            }
          } else {
            Serial.println("Auth: Failed");
            return request->requestAuthentication();
          }
        });

        _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
          Serial.println("HTTP_GET: '/' (index.html)");
          request->send_P(200, "text/html", _rootIndexHtml);
        });

        /** a.bin == app.js */
        _server.on("/a.bin", HTTP_GET, [](AsyncWebServerRequest *request) {
          Serial.println("HTTP_GET: '/a.bin (app.js)'");
          AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/a.bin.gz", "text/javascript", false);
          response->addHeader("Content-Encoding", "gzip");
          request->send(response);
        });

        // _server.on("/ip.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        //   Serial.println("HTTP_GET: '/ip.js'");
        //   IPAddress ip = getStationIp();
        //   char msg[35];
        //   // LEFT OFF HERE> Perhaps have the client request a WebSocket connection and we connect from the server using their IP from the request.
        //   // LEFT OFF HERE> Need to give AP IP if connected to the access point.
        //   sprintf(&msg[0], "window.__ip='%s';", ip.toString().c_str());
        //   request->send(200, "text/html", &msg[0]);
        // });

        /** b.bin == favicon.ico */
        _server.on("/b.bin", HTTP_GET, [](AsyncWebServerRequest *request) {
          Serial.println("HTTP_GET: '/b.bin (favicon.ico)'");
          AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/b.bin.gz", "image/x-icon", false);
          response->addHeader("Content-Encoding", "gzip");
          request->send(response);
        });

        _server.onNotFound([](AsyncWebServerRequest *request) {
          lg.trace(F("Page Not Found\n"));
          IPAddress remote_ip = request->client()->remoteIP();
          request->send(404, "text/plain", "404 Error - Page Not found");
        });

        /** HTTP_GET: '/ota'
        ******************************************************************************************/

        _server.on("/ota", HTTP_GET, [](AsyncWebServerRequest *request) {
          Serial.println("HTTP_GET: '/ota'");
          request->send_P(200, "text/html", _otaFirmwareIndexHtml);
        });

        // LEFT OFF HERE> Adding spiffs.
        _server.on("/ota-spiff", HTTP_GET, [](AsyncWebServerRequest *request) {
          Serial.println("HTTP_GET: '/ota'");
          request->send_P(200, "text/html", _otaFirmwareIndexHtml);
        });

        /** HTTP_POST: '/ota-update'
        ******************************************************************************************/

        _server.on("/ota-update", HTTP_POST,
          
          [](AsyncWebServerRequest *request) {
            lg.trace(F("HTTP_POST: \"/ota-update\"" CR));
            // The request handler is triggered after the upload has finished... 
            // Create the response, add header, and send response
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
            response->addHeader("Connection", "close");
            response->addHeader("Access-Control-Allow-Origin", "*");
            request->send(response);
          },

          /** OTA Updater */
          [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

            // Upload handler chunks in data.
            if (!index) {
              Serial.printf("UploadStart: %s\n", filename.c_str());
              // calculate sketch space required for the update
              uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
              _otaState = NET_JOB_STATUS::START;
              _onNetStatusUpdateCb(NET_JOB::OTA, _otaState);

              // Start with max available size
              if (!Update.begin(maxSketchSpace)) {
                Update.printError(Serial);
              }
            }

            // Write chunked data to the free sketch space
            if (Update.write(data, len) != len) {
              Update.printError(Serial);
            }

            // If the final flag is set then this is the last frame of data
            if (final) {

              // True to set the size to the current progress
              if (Update.end(true)) {
                _otaState = NET_JOB_STATUS::COMPLETE;
                _onNetStatusUpdateCb(NET_JOB::OTA, _otaState);
                _onNetStatusUpdateCb(NET_JOB::REBOOT, NET_JOB_STATUS::UNKNOWN);
              } else {
                _otaState = NET_JOB_STATUS::ERROR;
                Update.printError(Serial);
                _onNetStatusUpdateCb(NET_JOB::OTA, _otaState);
              }
            }
          }
        );

        _server.begin();
      }
    }

    void wifiConnectSta() {
      lg.verbose(F("task_network::wifiConnectSta()" CR));

      Serial.printf("wifiConnectSta() getMacAddrStr() = %s\n", getMacAddrStr().c_str());

      if (WiFi.isConnected() == true) {
        lg.trace(F("task_network::wifiConnectSta() WiFi already connected. Disconnecting now." CR));
        // WiFi.disconnect();
      }

      WiFi.begin(STATION_SSID, STATION_PASSWORD, WIFI_CHANNEL);

      uint8_t connectTries = 10;
      while ((connectTries > 0) && (WiFi.status() != WL_CONNECTED)) {
        /** @todo Remove this delay and use a timer */
        delay(50);
        connectTries--;
      }
    }

    // LEFT OFF HERE> Put IP Addresses in a stats area on the web client.

    void startAsStandardBaseUnit() {
      lg.verbose(F("task_network::startAsStandardBaseUnit(...)" CR));
      reset();

      WiFi.onEvent(onWifiEvent);

      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP(DEFAULT_BASE_AP_SSID, DEFAULT_BASE_AP_PASSWORD, WIFI_CHANNEL, false);
      wifiConnectSta();

      // if (WiFi.status() == WL_CONNECTED) {
      //   _onNetStatusUpdateCb(NET_JOB::STATION, NET_JOB_STATUS::CONNECTED);
      //   // _onNetStatusUpdateCb(NET_JOB::WIFI, NET_JOB_STATUS::CONNECTED);
      // }

      if (!MDNS.begin(_webAppUrl)) {
        lg.verbose(F("!o! mDNS Failed to start.\n"));
        return;
      } else {
        lg.verbose(F("\\o/ mDNS Started.\n"));
        MDNS.addService("http", "tcp", 80);
      }

      initWebSocket();
      setupWebServer();

      _tmrWifiCheck.start(TMR_MS_WIFI_CHECK_SLOW);

      _isInitialized = true;
    }

    /** WebSockets
     */
    void initWebSocket() {
      _ws.onEvent(onSocketIn);
      _server.addHandler(&_ws);
    }

    void onSocketIn(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType eventType,
                       void* arg, uint8_t* data, size_t len)
    {
      switch (eventType)
      {
      case WS_EVT_CONNECT:
        lg.verbose(F("ws-in | WS_EVT_CONNECT: #%u connected from %s\n"), client->id(), client->remoteIP().toString().c_str());
        _onSocketInCb(client, WS_EVT_CONNECT, NULL);
        // When a client connects send them all of our state information to get them started.
        // sendSocketMsg(client->id(), *_appState, true);
        break;

      case WS_EVT_DISCONNECT:
        lg.verbose(F("ws-in | WS_EVT_DISCONNECT: #%u disconnected\n"), client->id());
        _onSocketInCb(client, WS_EVT_DISCONNECT, NULL);
        break;

      case WS_EVT_DATA: {
        lg.verbose(F("ws-in | WS_EVT_DATA\n"));
        char strMsg[len + 1]; // May not need the +1

        if (len > SCK_MSG_SIZE) {
          lg.error(F("Incoming socket message too large for buffer.\n"));
          return;
        }

        strncpy(&strMsg[0], (char *)data, len);
        strMsg[len] = '\0'; // Seem to need to to get rid of artifacs at end of string. Revisit.
        _onSocketInCb(client, WS_EVT_DATA, &(strMsg[0]));
        ParseResult parseResult = parseSocketDataIn(client, &(strMsg[0]), len);

        if (parseResult == ParseResult::PARSED_OK) {
          lg.verbose(F("PARSED_OK\n"));

        } else if (parseResult == ParseResult::PARSE_FAILED) {
          lg.verbose(F("PARSED_FAILED\n"));

        } else if (parseResult == ParseResult::NOTHING_CHANGED) {
          lg.verbose(F("NOTHING_CHANGED\n"));
        }

        break;
      }

      case WS_EVT_PONG:
        lg.verbose(F("ws-in | WS_EVT_PONG\n"));
        _onSocketInCb(client, WS_EVT_PONG, NULL);
        break;

      case WS_EVT_ERROR:
        lg.verbose(F("ws-in | WS_EVT_ERROR\n"));
        _onSocketInCb(client, WS_EVT_ERROR, NULL);
        break;

      }
    }

    ParseResult parseSocketDataIn(AsyncWebSocketClient* client, char* strMsg, size_t len) {
      lg.verbose(F("ws-in | raw data = |%s|\n"), strMsg);

      StaticJsonDocument<SCK_MSG_SIZE> doc;
      deserializeJson(doc, strMsg, len);

      // if (doc.containsKey("forceOff")) {
      //   _appState->setValForceOff(doc["forceOff"]);
      // }

      // if (doc.containsKey("forceOn")) {
      //   _appState->setValForceOn(doc["forceOn"]);
      // }

      // if (doc.containsKey("newLowWaterMark")) {
      //   _appState->setValLowWaterMark(doc["newLowWaterMark"]);
      // }

      // if (_appState->hasChangedValsForExternal()) {
      //   // Ensure all clients are updated with the latest changes.
      //   sendSocketMsg(*_appState);
      //   return ParseResult::PARSED_OK;
      // } else {
      //   return ParseResult::NOTHING_CHANGED;
      // }
    }

  } /** End Private */

  /**
  *  Public
  ******************************************************************************************/

  /** 
   *  @todo Break this up.
   *  @todo Hide the server. Currently issue with sensor and base seeing eachother with that.
   *  @todo If the Sensor AP is hidden it can't connect to base after the base is turned off and the sensor is left on.
   */

  // void setup(onNetStatusUpdateFn onNetStatusUpdateFn, bool hideServer) {
  //   lg.verbose(F("task_network::setup(...)" CR));

  //   _onNetStatusUpdateCb = onNetStatusUpdateFn;
  //   _hideApServer = hideServer;

  //   // WiFi
  //   WiFi.onEvent(onWifiEvent);
  //   WiFi.mode(wifi_mode_t::WIFI_MODE_APSTA);
  //   WiFi.softAP(SSID_AP, PWD_AP, WIFI_CHANNEL, false);

  //   setupWebServer();

  //   // Start timers
  //   // _tmrEspNow.start(TMR_MS_ESPNOW);
  //   // _tmrProcessMsgBuff.start(TMR_MS_PROCESS_MSG_BUFF);

  //   _isInitialized = true;
  // }

  void setup(onNetStatusUpdateFn onNetStatusUpdateCb, onSocketInFn onWsDataInCb, bool hideServer) {
    lg.verbose(F("task_network::setup(...)\n"));
    // _appState = appState;
    _onNetStatusUpdateCb = onNetStatusUpdateCb;
    _onSocketInCb = onWsDataInCb;
    _hideApServer = hideServer;
    startAsStandardBaseUnit();
  }

  bool isInitialized() {
    return _isInitialized;
  }

  /**
   *  Public
   */

  /**
   *  Routines
   */

  void reset() {
    _isInitialized = false;
    // Stop WiFi
    WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    _tmrWifiCheck.expire();
    // Stop the web server
    _server.end();

    /** @todo Look into removing this delay. May not need it and if we do, use a timer. */
    delay(100);
  }

  IPAddress getStationIp() {
    return WiFi.localIP();
  }

  IPAddress getAccessPointIp() {
    return WiFi.softAPIP();
  }

  bool isWifiConnected() {
    return WiFi.status() != WL_CONNECTED;
  }

  String getMacAddrStr() {
    return WiFi.macAddress();
  }

  uint8_t* getMacAddr() {
    WiFi.macAddress(_macAddr);
    return &_macAddr[0];
  }

  void getMacAddr(uint8_t* macAddr) {
    WiFi.macAddress(macAddr);
  }

  void saveIp(IPAddress ipAddr) {
    _ipAddr = ipAddr;

    if (ipAddr == IPAddress(0, 0, 0, 0)) {
      strcpy(_ipAddrTxt, "---.---.---.---");
    } else {
      sprintf(_ipAddrTxt, "...%d.%d.%d", _ipAddr[1], _ipAddr[2], _ipAddr[3]);
    }
  }

  int32_t getWifiChannel() {
    return WiFi.channel();
  }

  void sendSocketMsg(char* msg, bool forceAll) {
    sendSocketMsg(0, msg, forceAll);
  }

  void sendSocketMsg(uint32_t clientId, char* msg, bool forceAll) {
    // lg.verbose(F("task_net::sendSocketMsg()\n"));

    // StaticJsonDocument<SCK_MSG_SIZE> doc;

    // bool haveValuesToSend = false;

    // if (forceAll == true || appState.pump().isChangeHandlingRequiredForExternal() == true) {
    //   doc["pumpOn"] = appState.pump().getVal();
    //   appState.pump().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.waterLevel().isChangeHandlingRequiredForExternal() == true) {
    //   doc["waterLevel"] = appState.waterLevel().getVal();
    //   appState.waterLevel().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.lowWaterMark().isChangeHandlingRequiredForExternal() == true) {
    //   doc["lowWaterMark"] = appState.lowWaterMark().getVal();
    //   appState.lowWaterMark().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.forceOff().isChangeHandlingRequiredForExternal() == true) {
    //   doc["forceOff"] = appState.forceOff().getVal();
    //   appState.forceOff().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.forceOn().isChangeHandlingRequiredForExternal() == true) {
    //   doc["forceOn"] = appState.forceOn().getVal();
    //   appState.forceOn().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.lowWaterMark().isChangeHandlingRequiredForExternal() == true) {
    //   doc["lowWaterMark"] = appState.lowWaterMark().getVal();
    //   appState.lowWaterMark().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.appStartedSecsAgo().isChangeHandlingRequiredForExternal() == true) {
    //   doc["appStartedSecsAgo"] = appState.appStartedSecsAgo().getVal();
    //   appState.appStartedSecsAgo().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.lastOnSecsAgo().isChangeHandlingRequiredForExternal() == true) {
    //   doc["lastOnSecsAgo"] = appState.lastOnSecsAgo().getVal();
    //   appState.lastOnSecsAgo().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.lastOffSecsAgo().isChangeHandlingRequiredForExternal() == true) {
    //   doc["lastOffSecsAgo"] = appState.lastOffSecsAgo().getVal();
    //   appState.lastOffSecsAgo().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.longestOffSecs().isChangeHandlingRequiredForExternal() == true) {
    //   doc["longestOffSecs"] = appState.longestOffSecs().getVal();
    //   appState.longestOffSecs().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (forceAll == true || appState.longestOnSecs().isChangeHandlingRequiredForExternal() == true) {
    //   doc["longestOnSecs"] = appState.longestOnSecs().getVal();
    //   appState.longestOnSecs().isChangeHandlingRequiredForExternal(false);
    //   haveValuesToSend = true;
    // }

    // if (haveValuesToSend == true) {
    //   serializeJson(doc, &_sckMsgBuff[0], sizeof(char*) * SCK_MSG_SIZE);
    //   if (clientId == 0) {
    //     _ws.textAll(&_sckMsgBuff[0]);
    //   } else {
    //     _ws.text(clientId, &_sckMsgBuff[0]);
    //   }
    // }
  }

  void setWebsiteUsername(char* username) {
    strncpy(&_websiteUsername[0], username, sizeof(_websiteUsername));
  }

  char* getWebsiteUsername() {
    return _websiteUsername;
  }

  void setWebsitePassword(char* password) {
    strncpy(&_websitePassword[0], password, sizeof(_websitePassword));
  }

  char* getWebsitePassword() {
    return _websitePassword;
  }

  void setApSsid(char* ssid) {
    strncpy(_apSsid, ssid, MAX_LEN_SSID_NAME);
  }

  char* getApSsid() {
    return &_apSsid[0];
  }

  void  setApPwd(char* pwd) {
    strncpy(_apPwd, pwd, MAX_LEN_SSID_PWD);
  }

  char* getApPwd() {
    return &_apPwd[0];
  }

  void setWebsiteUrl(char* url) {
    strncpy(_webAppUrl, url, MAX_LEN_WEB_APP_URL);
  }

  char* getWebsiteUrl() {
    return &_webAppUrl[0];
  }


}