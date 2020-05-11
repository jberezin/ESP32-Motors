#if __has_include("config/LocalConfig.h")
#include "config/LocalConfig.h"
#else
#include "config/DefaultConfig.h"
#endif

#include <Arduino.h>
#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <AsyncTCP.h>
#include "FileIO.h"
#include "WifiController.h"
#include "WebServer.h"
#include "udp_srv.h"
#include "ServoDriver.h"
#include "CommandLayer.h"
#include "OpBuffer.h"
#include <esp_log.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#ifdef SERVO
#define STATUSLEDPIN 13
#endif
#ifdef STEPPER
#define STATUSLEDPIN 17
#endif

String const TAG = "Main";
const char *host = "openMYR-esp32";

udp_srv *UDP_server;

// Water Sensor Code == begin

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  15 //Time ESP32 will go to sleep (in seconds)
//Define touch sensitivity. Greater the value, more the sensitivity.
#define Threshold 40

RTC_DATA_ATTR int bootCount = 0;
void callback(){
  //placeholder callback function
}
//Function that prints the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
    log_i("Wakeup Reason: %d", wakeup_reason);
  switch(wakeup_reason-1)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}


//Function that prints the touchpad by which ESP32 has been awaken from sleep
void print_wakeup_touchpad(){
 
  touch_pad_t touchPin;
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  //log_i("TouchPin: %d", touchPin);
  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}
// Water Sensor Code == end

void setup()
{
    esp_err_t err;
    pinMode(STATUSLEDPIN, OUTPUT);
    Serial.begin(115200);
    digitalWrite(STATUSLEDPIN, HIGH);
    delay(200);
    /*
    err = WifiController::init();
    if (err) log_i("Error: %s, #%u", esp_err_to_name(err), err);
    WebServer::init();
    delay(100);
    FileIO::init();
    delay(100);
    OpBuffer::getInstance();
    CommandLayer::getInstance()->init();
    UDP_server = new udp_srv();
    UDP_server->begin();

    //use mdns for host name resolution 
    if (!MDNS.begin(host))
    { 
        log_e("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    log_d("mDNS responder started");

    ArduinoOTA
        .onStart([]() {
            String type;
            if (UDP_server)
                UDP_server->end();
            WebServer::reset();
            CommandParser::stop_motors();

            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            log_d("Start updating " + type);
        })
        .onEnd([]() {
            log_d("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            log_d("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            log_e("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                log_e("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                log_e("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                log_e("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                log_e("Receive Failed");
            else if (error == OTA_END_ERROR)
                log_e("End Failed");
        });
    ArduinoOTA.setHostname(host);
    ArduinoOTA.begin();
*/
    digitalWrite(STATUSLEDPIN, LOW);
// begin water sensor code
    	//Increment boot number and print it every reboot
	++bootCount;
	log_i("Boot Number: %d", bootCount);
     //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();

  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();
   
  	//Set timer to 5 seconds
	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
	Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
	" Seconds");

  //Go to sleep now
  esp_deep_sleep_start();
    	 
// end water sensor code    
}

void loop()
{
    delay(1000);
    log_i("looping\n");
  //  ArduinoOTA.handle();
  //  UDP_server->prompt_broadcast();
	 
    // check sensor pin

 
    //log_i("OpBuffer Commands space in queue 1: %d", OpBuffer::getInstance()->opBufferCapacity(1));
}
