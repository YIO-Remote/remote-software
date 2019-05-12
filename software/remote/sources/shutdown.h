#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <QObject>

#include <stdio.h>
#include <wiringPi.h>
#include <mcp23017.h>

class Shutdown : public QObject
{
    Q_OBJECT

public:

    Q_INVOKABLE void shutdown()
    {
        wiringPiSetup () ;
        mcp23017Setup (100, 0x20);

        pinMode(109, OUTPUT);
        digitalWrite(109, LOW);
    }

};

#endif // SHUTDOWN_H
