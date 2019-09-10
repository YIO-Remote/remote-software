#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
#include <Preferences.h>
#include <ESPmDNS.h>

#include <service_ota.h>
#include <service_websocket.h>

OTA ota;
WebSocketAPI wsservice;

StaticJsonDocument<200> doc;
bool needsSetup = true;
////////////////////////////////////////////////////////////////
// STATE MACHINE
////////////////////////////////////////////////////////////////
int dockState = 0;

// 0 - needs setup
// 1 - connecting to wifi, turning on OTA, connecting to websocket
// 2 - succesful connection
// 3 - normal operation, LED off, turns on when charging
// 4 - error

////////////////////////////////////////////////////////////////
// BLUETOOTH SETUP
////////////////////////////////////////////////////////////////
BluetoothSerial DOCK_BT;
String message = "";        // the message
bool recordmessage = false; // if true, bluetooth will start recording messages

////////////////////////////////////////////////////////////////
// WIFI SETUP
////////////////////////////////////////////////////////////////
#define CONN_TIMEOUT 10    // wifi timeout
char hostString[16] = {0}; // stores the hostname
String ssid;               // ssid
String passwd;             // password
String remote_id;          // hostname of the remote
bool connected = false;
unsigned long WIFI_CHECK = 30000;

////////////////////////////////////////////////////////////////
// LED SETUP
////////////////////////////////////////////////////////////////
#define LED 23

bool led_setup = false; // if the LED brightness is adjusted from the remote, this is true

int max_brightness = 50; // this updates from the remote settings screen

int led_delay = map(max_brightness, 5, 255, 30, 5);
int led_pause = map(max_brightness, 5, 255, 800, 0);

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10; //Resolution 8, 10, 12, 15

TaskHandle_t LedTask; // core 0 task for handling LED stuff

////////////////////////////////////////////////////////////////
// CHARGING INDICATOR PIN
////////////////////////////////////////////////////////////////
#define CHG_PIN 13
bool charging = false;

void setCharging()
{
  if (digitalRead(CHG_PIN) == LOW)
  {
    charging = true;
  }
  else
  {
    charging = false;
  }
}

////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////
void ledHandleTask(void *pvParameters)
{
  // LED setup
  pinMode(LED, OUTPUT);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED, ledChannel);

  while (1)
  {
    vTaskDelay(2 / portTICK_PERIOD_MS);

    //if the remote is charging, pulsate the LED
    if (dockState == 3 && charging)
    {
      for (int dutyCycle = 0; dutyCycle <= max_brightness; dutyCycle++)
      {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(led_delay);
      }

      delay(led_pause);

      // decrease the LED brightness
      for (int dutyCycle = max_brightness; dutyCycle >= 0; dutyCycle--)
      {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(led_delay);
      }
      delay(1000);
    }
    else if (dockState == 0)
    {
      ledcWrite(ledChannel, 255);
      delay(800);
      ledcWrite(ledChannel, 0);
      delay(800);
    }
    else if (dockState == 1)
    {
      ledcWrite(ledChannel, 255);
      delay(300);
      ledcWrite(ledChannel, 0);
      delay(300);
    }
    else if (dockState == 2)
    {
      // Blink the LED 3 times to indicate successful connection
      for (int i = 0; i < 4; i++)
      {
        ledcWrite(ledChannel, 255);
        delay(100);
        ledcWrite(ledChannel, 0);
        delay(100);
      }
      dockState = 3;
    }
  }
}

void saveConfig(String data)
{
  DeserializationError error = deserializeJson(doc, data);

  if (error)
  {
    Serial.print(("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // check if there is an SSID and password
  if (doc.containsKey("ssid") && doc.containsKey("password") && doc.containsKey("remote_id"))
  {
    ssid = doc["ssid"].as<String>();
    passwd = doc["password"].as<String>();
    remote_id = doc["remote_id"].as<String>();

    Preferences preferences;
    preferences.begin("Wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("passwd", passwd);
    preferences.putString("remote", remote_id);
    preferences.putBool("valid", true);
    preferences.end();
  }
  else if (doc.containsKey("erase"))
  { // or erease the preferences
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
void setup()
{
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

  if (hasPref)
  {
    ssid = preferences.getString("ssid", "");
    passwd = preferences.getString("passwd", "");
    remote_id = preferences.getString("remote", "");

    if (ssid.equals("") || passwd.equals("") || remote_id.equals(""))
    {
      Serial.println("Invalid credidentials");
    }
    else
    {
      needsSetup = false;
      dockState = 1;
    }
  }

  preferences.end();

  // if need setup start the bluetooth server
  if (dockState == 0)
  {
    // Bluetooth begin
    DOCK_BT.begin(hostString);
  }
  else
  { // otherwsie connec to the Wifi network
    Serial.println("Connecting to wifi");

    WiFi.disconnect();
    // WiFi.enableSTA(true);
    // WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), passwd.c_str());

    int connCounter = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(300);
      Serial.print(".");
      connCounter += 1;

      // if cannot connect, go back to bluetooth server mode
      if (connCounter >= CONN_TIMEOUT)
      {
        Serial.println("Connection timeout");
        Preferences preferences;
        preferences.begin("Wifi", false);
        preferences.clear();
        preferences.end();

        ESP.restart();
      }
    }

    Serial.println("Successful connection");
    Serial.print("Got ip: ");
    Serial.println(WiFi.localIP());
    Serial.print("DNS: ");
    Serial.println(WiFi.dnsIP());

    if (!MDNS.begin(hostString))
    {
      Serial.println("Error setting up MDNS responder!");
      while (1)
      {
        delay(1000);
      }
    }
    Serial.println("mDNS started");

    // Add mDNS service
    MDNS.addService("yio-dock-ota", "tcp", 80);

    // initialize the OTA service
    ota.init();

    // connect to remote websocket API
    wsservice.connect(remote_id);
  }
}

////////////////////////////////////////////////////////////////
// LOOP
////////////////////////////////////////////////////////////////
void loop()
{
  // look for wifi credidentials on bluetooth when in setup mode
  if (DOCK_BT.available() && dockState == 0)
  {
    char incomingChar = DOCK_BT.read();
    if (String(incomingChar) == "{")
    {
      recordmessage = true;
      message = "";
    }
    else if (String(incomingChar) == "}")
    {
      recordmessage = false;
      message += "}";
      saveConfig(message);
    }
    if (recordmessage)
    {
      message += String(incomingChar);
    }
  }

  if (wsservice.connected && dockState == 1)
  {
    dockState = 2; //succesful connection
  }

  if (dockState > 0)
  {
    // OTA update
    ota.handle();

    // Websocket loop
    wsservice.loop();
  }

  delay(100);
}