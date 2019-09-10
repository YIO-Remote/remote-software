#ifndef SERVICE_WEBSOCKET_H
#define SERVICE_WEBSOCKET_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <service_ir.h>

class WebSocketAPI
{
public:
    void connect();
    void disconnect();
    void sendMessage(StaticJsonDocument<200> responseDoc);
    void loop();
    IPAddress findRemoteIP();

    bool connected = false;

private:
    InfraredService irservice;

    WebSocketsClient webSocket;
    bool remotefound = false;
    StaticJsonDocument<200> wsdoc;
    String token = "0";
};

#endif