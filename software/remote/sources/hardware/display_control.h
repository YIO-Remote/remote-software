#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <QObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include <stdio.h>
#include <stdint.h>
#ifdef __arm__
    #include <wiringPi.h>
    #include <mcp23017.h>
#endif
#include <time.h>

class DisplayControl : public QObject
{
    Q_OBJECT

    // define timing
    struct timespec ts = {0, 40L};
    struct timespec ts2 = {0, 100L};
    struct timespec ts3 = {0, 300L};


public:
    Q_INVOKABLE bool setmode(const QString &mode);

    Q_INVOKABLE void setBrightness(int from, int to);

    Q_INVOKABLE void batteryChargingOn();
    Q_INVOKABLE void batteryChargingOff();

    DisplayControl();

    void spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1);
    void setup(void);
};

#endif // DISPLAY_CONTROL_H
