#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <QObject>

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <mcp23017.h>
#include <time.h>

class DisplayControl : public QObject
{
    Q_OBJECT

    // define timing
    struct timespec ts = {0, 40L};
    struct timespec ts2 = {0, 100L};
    struct timespec ts3 = {0, 300L};


public:
    Q_INVOKABLE void setmode(const QString &mode);

    void spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1);
    void setup(void);

};


#endif // DISPLAY_CONTROL_H
