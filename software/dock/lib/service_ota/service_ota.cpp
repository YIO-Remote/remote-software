#include "service_ota.h"
#include <WebServer.h>
#include <Update.h>

WebServer OTAServer(9999);

void OTA::init()
{
    if (this->init_has_run)
    {
        return;
    }

    add_http(&OTAServer, "/update");
    OTAServer.begin(80);

    this->init_has_run = true;
}

void OTA::handle()
{
    if (!this->init_has_run)
    {
        OTA::init();
    }
    OTAServer.handleClient();
}

long OTA::max_sketch_size()
{
    long ret = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    return ret;
}

void OTA::add_http(WebServer *server, const char *path)
{
    server->on("/", HTTP_GET, [server]() {
        server->send(200, "text/html", "<h1>OTA Update</h1>");
    });

    server->on(path, HTTP_POST, [server,this]() {
		server->send(200, "text/plain", (Update.hasError()) ? "Update: fail\n" : "Update: OK!\n");
		delay(500);
		ESP.restart();
	}, [server,this]() {
		HTTPUpload& upload = server->upload();

		if (upload.status == UPLOAD_FILE_START) {
			Serial.printf("Firmware update initiated: %s\r\n", upload.filename.c_str());

			//uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			uint32_t maxSketchSpace = this->max_sketch_size();

			if (!Update.begin(maxSketchSpace)) { //start with max available size
				Update.printError(Serial);
			}
		} else if (upload.status == UPLOAD_FILE_WRITE) {
            if (!led_state) {
                led_state = true;
                ledcWrite(0, 255); 
            } else {
                led_state = false;
                ledcWrite(0, 0);
            }
			/* flashing firmware to ESP*/
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
				Update.printError(Serial);
			}

			// Store the next milestone to output
			uint16_t chunk_size  = 51200;
			static uint32_t next = 51200;

			// Check if we need to output a milestone (100k 200k 300k)
			if (upload.totalSize >= next) {
				Serial.printf("%dk ", next / 1024);
				next += chunk_size;
			}
		} else if (upload.status == UPLOAD_FILE_END) {
			if (Update.end(true)) { //true to set the size to the current progress
				Serial.printf("\r\nFirmware update successful: %u bytes\r\nRebooting...\r\n", upload.totalSize);
			} else {
				Update.printError(Serial);
			}
		}
	});

    server->begin();
}
