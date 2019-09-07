#include "service_websocket.h"
#include <ESPmDNS.h>

void WebSocket::connect()
{
    // find the dock
    int n = MDNS.queryService("yio-api", "tcp");
    if (n == 0) {
        Serial.println("No remote api found");
    } else {
        Serial.println("Found: ");
        Serial.println(MDNS.hostname(0));
    }
}

void WebSocket::disconnect()
{

}

void WebSocket::eventHandler()
{

}

void WebSocket::sendMessage()
{

}