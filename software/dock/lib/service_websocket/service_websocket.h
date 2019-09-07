#ifndef SERVICE_WEBSOCKET_H
#define SERVICE_WEBSOCKET_H

#include <Arduino.h>
#include <WebSocketsClient.h>

class WebSocketAPI
{
public:
    void connect(IPAddress ip);
    void disconnect();
    void sendMessage();
    static void eventHandler(WStype_t type, uint8_t *payload, size_t length);
    void loop();

private:
    WebSocketsClient webSocket;
    bool remotefound = false;
};

#endif