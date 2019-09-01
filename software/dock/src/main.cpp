#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
#include <Preferences.h>
#include <ESPmDNS.h>

#include <service_ir.h>
// #include <service_tcp.h>

// TCPService tcpservice;

StaticJsonDocument<200> doc;
bool needsSetup = true;

////////////////////////////////////////////////////////////////
// BLUETOOTH SETUP
////////////////////////////////////////////////////////////////
BluetoothSerial DOCK_BT;        
String message = "";            // the message
bool recordmessage = false;     // if true, bluetooth will start recording messages

////////////////////////////////////////////////////////////////
// WIFI SETUP
////////////////////////////////////////////////////////////////
#define CONN_TIMEOUT 10         // wifi timeout
char hostString[16] = {0};      // stores the hostname
String ssid;                    // ssid
String passwd;                  // password

////////////////////////////////////////////////////////////////
// LED SETUP
////////////////////////////////////////////////////////////////
#define LED 23

bool led_setup = false;         // if the LED brightness is adjusted from the remote, this is true

int max_brightness = 50;        // this updates from the remote settings screen

int led_delay = map(max_brightness, 5, 255, 30, 5);
int led_pause = map(max_brightness, 5, 255, 800, 0);

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10; //Resolution 8, 10, 12, 15

TaskHandle_t LedTask;           // core 0 task for handling LED stuff

////////////////////////////////////////////////////////////////
// CHARGING INDICATOR PIN
////////////////////////////////////////////////////////////////
#define CHG_PIN 13
bool charging = false;

void setCharging() {
  if (digitalRead(CHG_PIN) == LOW) {
    charging = true;  
  } else {
    charging = false;
  }
}

////////////////////////////////////////////////////////////////
// IR SETUP
////////////////////////////////////////////////////////////////
InfraredService irservice;

////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////
void ledHandleTask(void * pvParameters) {
  // LED setup
  pinMode(LED, OUTPUT);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED, ledChannel);

  while(1) {
    vTaskDelay(2 / portTICK_PERIOD_MS);

    //if the remote is charging, pulsate the LED
    if (charging && !led_setup) {
      for (int dutyCycle = 0; dutyCycle <= max_brightness; dutyCycle++) {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(led_delay);
      }

      delay(led_pause);

      // decrease the LED brightness
      for (int dutyCycle = max_brightness; dutyCycle >= 0; dutyCycle--) {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(led_delay);
      }
      delay(1000);
    } 
    if (needsSetup) {
      ledcWrite(ledChannel, 255);
      delay(800);
      ledcWrite(ledChannel, 0);
      delay(800);
    }  
  }
}

void saveConfig(String data) {
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;  
  }

  // check if there is an SSID and password
  if (doc.containsKey("ssid") && doc.containsKey("password")) {
    ssid = doc["ssid"].as<String>();
    passwd = doc["password"].as<String>();

    Preferences preferences;
    preferences.begin("Wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("passwd", passwd);
    preferences.putBool("valid", true);
    preferences.end();

  } else if (doc.containsKey("erase")) { // or erease the preferences
    Preferences preferences;
    preferences.begin("Wifi", false);
    preferences.clear();
    preferences.end();
  }

  ESP.restart();
}

////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  // run LED handling on other core
  xTaskCreatePinnedToCore(ledHandleTask, "LedTask", 10000, NULL, 1, &LedTask, 0);

  // Generate a name for the network
  sprintf(hostString, "YIO-Dock-%06X", ESP.getEfuseMac());

  // CHARGING PIN setup
  pinMode(CHG_PIN, INPUT);
  attachInterrupt(CHG_PIN, setCharging, CHANGE);

  // check if there are ssid and password stored
  Preferences preferences;
  preferences.begin("Wifi", false);
  bool hasPref = preferences.getBool("valid", false);

  if (hasPref) {
    ssid = preferences.getString("ssid", "");
    passwd = preferences.getString("passwd", "");

    if (ssid.equals("") || passwd.equals("")) {
      Serial.println("Invalid credidentials");
    } else {
      needsSetup = false;
    }
  }

  preferences.end();

// if need setup start the bluetooth server
 if (needsSetup) {
    // Bluetooth begin
    DOCK_BT.begin(hostString);

  } else { // otherwsie connec to the Wifi network
    Serial.println("Connecting to wifi");

    WiFi.disconnect(true);
    WiFi.enableSTA(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), passwd.c_str());

    int connCounter = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
      ledcWrite(ledChannel, 255);
      delay(300);
      ledcWrite(ledChannel, 0);
      delay(300);
      Serial.print(".");
      connCounter += 1;

      // if cannot connect, go back to bluetooth server mode
      if (connCounter >= CONN_TIMEOUT) {
        Serial.println("Connection timeout");
        Preferences preferences;
        preferences.begin("Wifi", false);
        preferences.clear();
        preferences.end();

        ESP.restart();
      }
    }

    Serial.println("Successful connection");

    if (!MDNS.begin(hostString)) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    Serial.println("mDNS started");

    // start server
    // tcpservice.setupTCP();

    // Add mDNS service
    MDNS.addService("yio-dock-http", "tcp", 80);
    MDNS.addService("yio-dock-telnet", "tcp", 23);

    // Blink the LED 3 times to indicate successful connection
    for (int i=0; i<4; i++) {
      ledcWrite(ledChannel, 255);
      delay(100);
      ledcWrite(ledChannel, 0);
      delay(100);
    }

    // initialize the IR service
    irservice.init();
  }
}

////////////////////////////////////////////////////////////////
// LOOP
////////////////////////////////////////////////////////////////
void loop() {
  // look for wifi credidentials on bluetooth when in setup mode
  if (DOCK_BT.available()) {
    char incomingChar = DOCK_BT.read();
    if (String(incomingChar) == "{") {
      recordmessage = true;
      message = "";
    } else if (String(incomingChar) == "}") {
      recordmessage = false;
      message += "}";
      saveConfig(message);
    }
    if (recordmessage) {
      message += String(incomingChar);
    }
  }
  // tcpservice.handleTCP();
  // IR Receive
  if (irservice.receiving){
    irservice.receive();    
  }
}