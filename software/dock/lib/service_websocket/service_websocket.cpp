#include "service_websocket.h"
#include <ESPmDNS.h>
#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>

void WebSocketAPI::connect(String hostname)
{
    // initialize the IR service
    irservice.init();

    IPAddress serverip = findRemoteIP(hostname);
    webSocket.begin(serverip, 946, "/");
    webSocket.setReconnectInterval(5000);
    webSocket.onEvent([&](WStype_t type, uint8_t *payload, size_t length) {
        switch (type)
        {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            connected = false;
            break;
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);

            deserializeJson(wsdoc, payload);

            // YIO API AUTHENTICATION
            if (wsdoc.containsKey("type") && wsdoc["type"].as<String>() == "auth_required") {
                StaticJsonDocument<200> responseDoc;
                responseDoc["type"] = "auth";
                responseDoc["token"] = token;
                String message;
                serializeJson(responseDoc, message);
                webSocket.sendTXT(message);
            }
            if (wsdoc.containsKey("type") && wsdoc["type"].as<String>() == "auth_ok") {
                StaticJsonDocument<200> responseDoc;
                responseDoc["type"] = "dock";
                responseDoc["message"] = "connected";
                String message;
                serializeJson(responseDoc, message);
                webSocket.sendTXT(message);
                Serial.println("OK");
                connected = true;
            }
            // RECEIVE IR CODE FROM REMOTE AND EMIT IT
            // {"type": "dock",
            //  "command": "ir_send",
            //  "code": "0000,0067,0000,0015,0060,0018"
            // }
            if (wsdoc.containsKey("type") && wsdoc["type"].as<String>() == "dock") {
                // Change LED brightness
                if (wsdoc["command"].as<String>() == "led_brightness_start") {
                    led_setup = true;
                    led_brightness = wsdoc["brightness"].as<int>();
                }
                if (wsdoc["command"].as<String>() == "led_brightness_stop") {
                    led_setup = false;
                }

                // Send IR code
                if (wsdoc["command"].as<String>() == "ir_send") {
                    irservice.send(wsdoc["code"].as<String>());
                }

                // Turn on IR receiving
                    if (wsdoc["command"].as<String>() == "ir_receive_on") {
                    irservice.receiving = true;    
                }

                // Turn on IR receiving
                if (wsdoc["command"].as<String>() == "ir_receive_off") {
                    irservice.receiving = false;    
                }

                // Erase and reset the dock
                if (wsdoc["command"].as<String>() == "erase") {
                    Preferences preferences;
                    preferences.begin("Wifi", false);
                    preferences.clear();
                    preferences.end();   

                    delay(500);
                    
                    preferences.begin("LED", false);
                    preferences.clear();
                    preferences.end();

                    int err;
                    err=nvs_flash_init();
                    Serial.println("nvs_flash_init: " + err);
                    err=nvs_flash_erase();
                    Serial.println("nvs_flash_erase: " + err);

                    delay(500);
                    ESP.restart();
                }
            }
            break;

        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
        case WStype_BIN:
        case WStype_PING:
        case WStype_PONG:
            break;
        }
    });
}

void WebSocketAPI::disconnect()
{
    webSocket.disconnect();
}

void WebSocketAPI::sendMessage(StaticJsonDocument<200> responseDoc)
{
    String message;
    serializeJson(responseDoc, message);
    webSocket.sendTXT(message);
}

void WebSocketAPI::loop()
{
    webSocket.loop();

    //IR Receive
    if (irservice.receiving)
    {
        String code_received = irservice.receive();

        if (code_received != "") {
        StaticJsonDocument<500> responseDoc;
        responseDoc["type"] = "dock";
        responseDoc["command"] = "ir_receive";
        responseDoc["code"] = code_received;

        String message;
        serializeJson(responseDoc, message);
        webSocket.sendTXT(message);
        Serial.println(message);
        Serial.println("OK");    
        }
    }
}

IPAddress WebSocketAPI::findRemoteIP(String hostname)
{
    IPAddress serverip = MDNS.queryHost(hostname);

    while (serverip.toString() == "0.0.0.0")
    {
        Serial.println("Trying again to resolve mDNS");
        delay(250);
        serverip = MDNS.queryHost(hostname);
    }

    Serial.print("IP address of server: ");
    Serial.println(hostname);
    Serial.println(serverip.toString());

    return serverip;
}

// IPAddress WebSocketAPI::findRemoteIP()
// {
//     String hostname;

//     Serial.println("Finding YIO Remote host");

//     while (!remotefound)
//     {
//         delay(100);
//         Serial.println("Finding YIO Remote host");

//         int n = MDNS.queryService("yio-remote", "tcp");
//         if (n != 0)
//         {
//             hostname = MDNS.hostname(0);
//             remotefound = true;
//         }
//     }

//     IPAddress serverip = MDNS.queryHost(hostname);

//     while (serverip.toString() == "0.0.0.0")
//     {
//         Serial.println("Trying again to resolve mDNS");
//         delay(250);
//         serverip = MDNS.queryHost(hostname);
//     }

//     Serial.print("IP address of server: ");
//     Serial.println(hostname);
//     Serial.println(serverip.toString());

//     return serverip;
// }