#include <QtDebug>

#include "display_control.h"

#define CLK 107 // GPIO 7
#define MOSI 106 // GPIO 6
#define CS 105 // GPIO 6
#define RST 104 // GPIO 4

void DisplayControl::setup(void)
{
    qDebug() << "spi screen setup function called";
    wiringPiSetup () ;
    mcp23017Setup (100, 0x20);
}

void DisplayControl::spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1)
{
    qDebug() << "spi screenreg function called";
    int32_t i;
    int32_t control_bit;

    pinMode(MOSI, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(CS, OUTPUT);
    nanosleep(&ts, NULL);
    nanosleep(&ts, NULL);

    digitalWrite(CS, HIGH); //digitalWrite(CS, HIGH);
    digitalWrite(MOSI, HIGH);
    digitalWrite(CLK, LOW);
    nanosleep(&ts2, NULL);

    digitalWrite(CS, LOW);
    control_bit = 0x0000;
    Addr = (control_bit | Addr);
    // printf("addr is 0x%x \n", Addr);
    for(i = 0; i<9; i++)
    {
        if(Addr &(1<<(8-i)))
            digitalWrite(MOSI, HIGH);
        else
            digitalWrite(MOSI, LOW);

        // \u6a21\u62dfCLK
        digitalWrite(CLK, HIGH);
        nanosleep(&ts, NULL);
        digitalWrite(CLK, LOW);
        nanosleep(&ts, NULL);
    }

    digitalWrite(CS, HIGH);
    digitalWrite(MOSI, HIGH);
    digitalWrite(CLK, LOW);
    nanosleep(&ts3, NULL);

    if(0xffff == Data0){
        return;
    }

    digitalWrite(CS, LOW);

    control_bit = 0x0100;
    Data0 = (control_bit | Data0);
    //printk("data0 is 0x%x \n", Data);
    for(i = 0; i < 9; i++)  //data
    {
        if(Data0 &(1<<(8-i)))
            digitalWrite(MOSI, HIGH);
        else
            digitalWrite(MOSI, LOW);
        digitalWrite(CLK, HIGH);
        nanosleep(&ts, NULL);
        digitalWrite(CLK, LOW);
        nanosleep(&ts, NULL);
    }

    digitalWrite(CS, HIGH);
    digitalWrite(CLK, LOW);
    digitalWrite(MOSI, LOW);
    nanosleep(&ts2, NULL);

    if(0xffff == Data1)
        return;

    digitalWrite(CS, LOW);

    control_bit = 0x0100;
    Data1 = (control_bit | Data1);
//    printk("data1 is 0x%x \n", Data);
    for(i = 0; i < 9; i++)  //data
    {
        if(Data1 &(1<<(8-i)))
            digitalWrite(MOSI, HIGH);
        else
            digitalWrite(MOSI, LOW);
        digitalWrite(CLK, HIGH);
        nanosleep(&ts, NULL);
        digitalWrite(CLK, LOW);
        nanosleep(&ts, NULL);
    }

    digitalWrite(CS, HIGH);
    digitalWrite(CLK, LOW);
    digitalWrite(MOSI, LOW);
    nanosleep(&ts3, NULL);
}

void DisplayControl::setmode(const QString &mode)
{
    if (mode == "standbyon") {
        qDebug() << "spi standby on function called";
        spi_screenreg_set(0x10, 0xffff, 0xffff);
        delay(120);
        spi_screenreg_set(0x28, 0xffff, 0xffff);
    }
    if (mode == "standbyoff") {
        qDebug() << "spi standby off function called";
        spi_screenreg_set(0x29, 0xffff, 0xffff);
        spi_screenreg_set(0x11, 0xffff, 0xffff);
    }
}
