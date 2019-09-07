#include "service_websocket.h"
#include <ESPmDNS.h>
#include <ArduinoJson.h>

void WebSocketAPI::eventHandler(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);

        // send message to server when Connected
        // webSocket.sendTXT("Connected");
        break;
    case WStype_TEXT:
        Serial.printf("[WSc] get text: %s\n", payload);

        // send message to server
        // webSocket.sendTXT("message here");
        break;

    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

void WebSocketAPI::connect(IPAddress ip)
{
    webSocket.begin(ip, 496, "/");
    webSocket.onEvent(WebSocketAPI::eventHandler);
    webSocket.setReconnectInterval(5000);
}

void WebSocketAPI::disconnect()
{
}

void WebSocketAPI::sendMessage()
{
}

void WebSocketAPI::loop()
{
    IPAddress serverIp;
    String hostname;

    // find the dock
    if (!remotefound)
    {
        Serial.println("Looking for a remote");
        int n = MDNS.queryService("yio-remote", "tcp");
        if (n == 0)
        {
            Serial.println("No remote api found");
        }
        else
        {
            hostname = MDNS.hostname(0);
            serverIp = MDNS.queryHost(hostname);

            Serial.println("Found: ");
            Serial.println(hostname);
            // Serial.println(MDNS.IP(0));
            remotefound = true;
        }
    }
    else
    {
        while (serverIp.toString() == "0.0.0.0")
        {
            Serial.println("Trying again to resolve mDNS");
            delay(250);
            serverIp = MDNS.queryHost(hostname);
        }
        Serial.print("IP address of server: ");
        Serial.println(serverIp.toString());
    }
    webSocket.loop();
}