#ifndef SERVICE_WEBSOCKET_H
#define SERVICE_WEBSOCKET_H

#include <Arduino.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

class WebSocket {
    public:
    void connect();
    void disconnect();
    void sendMessage();
    void eventHandler();
};

#endif