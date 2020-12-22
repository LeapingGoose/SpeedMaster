/*
     esp32now slave 
*/

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define HIGHEST_CHANNEL	11
#define LOWEST_CHANNEL  1
#define FIXED_CHANNEL		6

//#define USE_FIXED_CHANNEL   //Uncomment for no channel search


//Use custom MAC for both master and slave
uint8_t masterDeviceMac[] = {0xB4, 0xE6, 0x2D, 0xE9, 0xFE, 0x6E};  
uint8_t CustomMac[] = {0x3C, 0x71, 0xBF, 0x03, 0x3D, 0x30};
esp_now_peer_info_t master;
const esp_now_peer_info_t *masterNode = &master;
esp_err_t sendResult;
const uint8_t maxDataFrameSize = 200;
uint8_t dataToSend[maxDataFrameSize];
byte cnt = 0;
int dataSent = 0;

int wifi_channel = 1;

int ms_sleep = 0;

void WiFiReset() {
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
}


void setup()
{
  Serial.begin(115200);
  Serial.print("\r\n\r\n");
 
#ifdef USE_FIXED_CHANNEL
  uint8_t primaryChan = FIXED_CHANNEL;
#else
  for (uint8_t primaryChan = LOWEST_CHANNEL; primaryChan <= HIGHEST_CHANNEL; primaryChan++) {
#endif
    WiFiReset();
    WiFi.mode(WIFI_STA);
    
    delay(100);
    
    esp_wifi_set_mac(ESP_IF_WIFI_STA, &CustomMac[0]);
  
    delay(250);
  
    Serial.print("Channel no: ");
    Serial.println(primaryChan);
    Serial.println( WiFi.macAddress() );
    
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    wifi_second_chan_t secondChan = WIFI_SECOND_CHAN_NONE;
    ESP_ERROR_CHECK(esp_wifi_set_channel(primaryChan, secondChan));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
  
    WiFi.printDiag(Serial);
  
    WiFi.disconnect();

    Serial.print("Wifi Channel: "); Serial.println(WiFi.channel());

    if (esp_now_init() == ESP_OK)
    {
      Serial.println("ESP NOW INIT!");
    }
    else
    {
      Serial.println("ESP NOW INIT FAILED....");
    }

    memcpy( &master.peer_addr, masterDeviceMac, 6 );
    master.channel = primaryChan;
    master.encrypt = 0;
    master.ifidx = ESP_IF_WIFI_STA;

#ifdef USE_FIXED_CHANNEL
    if ( esp_now_add_peer(masterNode) == ESP_OK)
    {
      Serial.println("Added Peer!");
    }
#else
    //Add node irst time, else  replace
    if (primaryChan == LOWEST_CHANNEL) {
      if ( esp_now_add_peer(masterNode) == ESP_OK)
      {
        Serial.println("Added Peer!");
      }
    } else {
      if (esp_now_mod_peer(masterNode) == ESP_OK)
      {
        Serial.println("Modified Peer!");
      }
    }
#endif

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    dataSent = 0;

    //Send test data
    if ( TestSend() ) {

      //Wait for data sent confirmed
      for (int i = 0; i < 1000; i++, ms_sleep--) {
        delay(1);
        yield();
        if (dataSent != 0 ) {
          break;								//No data sent, try another channel
        }
      }
      if (dataSent == 1 ) {
        //Save channel to nvr/RTC mem ?
        Serial.printf("Found Master on channel: %d\n", WiFi.channel());
#ifndef USE_FIXED_CHANNEL
        break;
#endif
      }
    }
#ifndef USE_FIXED_CHANNEL
    delay(500);
  }
#endif
}

void loop()
{
  for (cnt = 0; cnt < maxDataFrameSize; cnt++)
  {
    dataToSend[cnt]++;
  }
  
  esp_err_t sendResult = esp_now_send(master.peer_addr, dataToSend, sizeof(dataToSend)); //maxDataFrameSize);
	if (sendResult == ESP_OK) {
		Serial.println("Success");
	} else if (sendResult == ESP_ERR_ESPNOW_NOT_INIT) {
		// How did we get so far!!
		Serial.println("ESPNOW not Init.");
	} else if (sendResult == ESP_ERR_ESPNOW_ARG) {
		Serial.println("Invalid Argument");
	} else if (sendResult == ESP_ERR_ESPNOW_INTERNAL) {
		Serial.println("Internal Error");
	} else if (sendResult == ESP_ERR_ESPNOW_NO_MEM) {
		Serial.println("ESP_ERR_ESPNOW_NO_MEM");
	} else if (sendResult == ESP_ERR_ESPNOW_NOT_FOUND) {
		Serial.println("Peer not found.");
	} 
	else if (sendResult == ESP_ERR_ESPNOW_IF) {
		Serial.println("Interface Error.");
	}   else {
		Serial.printf("\r\nNot sure what happened\t%d", sendResult);
	}
	
  delay(1500);
}

int TestSend() {
  
  strcpy((char*)dataToSend, "Calling Master");

	esp_err_t sendResult = esp_now_send(master.peer_addr, dataToSend, sizeof(dataToSend)); //maxDataFrameSize);
	if (sendResult == ESP_OK) {
		Serial.println("Success");
		return 1;
	} else if (sendResult == ESP_ERR_ESPNOW_NOT_INIT) {
		// How did we get so far!!
		Serial.println("ESPNOW not Init.");
	} else if (sendResult == ESP_ERR_ESPNOW_ARG) {
		Serial.println("Invalid Argument");
	} else if (sendResult == ESP_ERR_ESPNOW_INTERNAL) {
		Serial.println("Internal Error");
	} else if (sendResult == ESP_ERR_ESPNOW_NO_MEM) {
		Serial.println("ESP_ERR_ESPNOW_NO_MEM");
	} else if (sendResult == ESP_ERR_ESPNOW_NOT_FOUND) {
		Serial.println("Peer not found.");
	} 
	else if (sendResult == ESP_ERR_ESPNOW_IF) {
		Serial.println("Interface Error.");
	}   else {
		Serial.printf("\r\nNot sure what happened\t%d", sendResult);
	}
  return 0;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  dataSent = (status == ESP_NOW_SEND_SUCCESS ? 1 : -1);
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  //Serial.printf("\r\nReceived\t%d Bytes\t%d\n", data_len, data[0]);
  Serial.printf("\r\nReceived\t%d Bytes\t%s\n", data_len, data);
}