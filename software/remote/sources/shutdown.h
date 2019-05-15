#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <QObject>

#include <stdio.h>
#ifdef __linux__
#include <wiringPi.h>
#include <mcp23017.h>
#endif

class Shutdown : public QObject
{
    Q_OBJECT

public:

    Q_INVOKABLE void shutdown()
    {
#ifdef __linux__
        wiringPiSetup () ;
        mcp23017Setup (100, 0x20);

        pinMode(109, OUTPUT);
        digitalWrite(109, LOW);
#endif
    }

};

#endif // SHUTDOWN_H
