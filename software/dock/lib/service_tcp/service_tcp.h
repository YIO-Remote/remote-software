#ifndef SERVICE_TCP_H
#define SERVICE_TCP_H

#include <WiFi.h>
#include <service_ir.h>

WiFiServer server(23);

class TCPService
{
public:
    void setupTCP()
    {
        server.begin();
    }

    void handleTCP()
    {
        WiFiClient tcpClient = server.available();

        if (tcpClient)
        {
            if (tcpClient.connected())
            {
                Serial.println("Client Connected");
            }

            while (tcpClient.connected())
            {
                while (tcpClient.available() > 0)
                {
                    // read data from the connected client
                    String pronto = tcpClient.readStringUntil('\r');
                    boolean didSend = irservice.handlePronto(pronto);
                    if (didSend)
                    {
                        //tcpClient.write("1\r");
                    }
                    else
                    {
                        //tcpClient.write("Error!\r\n");
                    }
                }
                //Send Data to connected client
                while (Serial.available() > 0)
                {
                    tcpClient.write(Serial.read());
                }
            }
            tcpClient.stop();
            Serial.println("Client disconnected");
        }
    }

public:
    InfraredService irservice;
};

#endif