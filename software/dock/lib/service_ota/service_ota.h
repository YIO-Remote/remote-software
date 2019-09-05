#ifndef SERVICE_OTA_H
#define SERVICE_OTA_H

#include <Arduino.h>
#include <WebServer.h>

class OTA
{
public:
    unsigned int port;
    String path = "";

    void init();
    void add_http(WebServer *server, const char *path);
    void handle();

private:
    bool init_has_run;
    long max_sketch_size();
    bool led_state = false;
};

#endif