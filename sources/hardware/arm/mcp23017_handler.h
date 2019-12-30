/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#ifndef HARDWARE_ARM_MCP23017_HANDLER_H_
#define HARDWARE_ARM_MCP23017_HANDLER_H_

#include <QString>

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define MCP23017_ADDRESS 0x20

// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14

#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define MCP23017_INT_ERR 255

class MCP23017 {
 public:
    MCP23017() { setup(); }

    ~MCP23017() {}

    void setup() {
        wiringPiSetup();

        // TODO(zehnm) make i2c device configurable
        bus = wiringPiI2CSetupInterface("/dev/i2c-3", MCP23017_ADDRESS);

        // set up all inputs on both ports
        wiringPiI2CWriteReg8(bus, MCP23017_IODIRA, 0xbf);  // 0xbf 0b10111111
        wiringPiI2CWriteReg8(bus, MCP23017_IODIRB, 0xff);

        // set up interrupts
        int ioconfValue = wiringPiI2CReadReg8(bus, MCP23017_IOCONA);
        bitWrite(ioconfValue, 6, true);
        bitWrite(ioconfValue, 2, false);
        bitWrite(ioconfValue, 1, false);
        wiringPiI2CWriteReg8(bus, MCP23017_IOCONA, ioconfValue);

        ioconfValue = wiringPiI2CReadReg8(bus, MCP23017_IOCONB);
        bitWrite(ioconfValue, 6, true);   // mirror
        bitWrite(ioconfValue, 2, false);  //
        bitWrite(ioconfValue, 1, false);  // polarity
        wiringPiI2CWriteReg8(bus, MCP23017_IOCONB, ioconfValue);

        // setup pin for interrupt
        wiringPiI2CWriteReg8(bus, MCP23017_INTCONA, 0x00);
        wiringPiI2CWriteReg8(bus, MCP23017_GPPUA, 0xbf);

        wiringPiI2CWriteReg8(bus, MCP23017_INTCONB, 0x00);
        wiringPiI2CWriteReg8(bus, MCP23017_GPPUB, 0xff);

        // enable pin for interrupt
        wiringPiI2CWriteReg8(bus, MCP23017_GPINTENA, 0xbf);
        wiringPiI2CWriteReg8(bus, MCP23017_GPINTENB, 0xff);

        wiringPiI2CReadReg8(bus, MCP23017_INTCAPA);
        wiringPiI2CReadReg8(bus, MCP23017_INTCAPB);
    }

    QString readInterrupt() {
        int intf;

        intf = wiringPiI2CReadReg8(bus, MCP23017_INTFA);

        switch (intf) {
            case 0x01:
                clearInterrupt();
                return "apds9960";
            case 0x02:
                clearInterrupt();
                return "dpad up";
            case 0x04:
                clearInterrupt();
                return "top right";
            case 0x08:
                clearInterrupt();
                return "channel up";
            case 0x10:
                clearInterrupt();
                return "dpad right";
            case 0x20:
                clearInterrupt();
                return "channel down";
            case 0x80:
                clearInterrupt();
                return "battery";
        }

        intf = wiringPiI2CReadReg8(bus, MCP23017_INTFB);

        switch (intf) {
            case 0x01:
                clearInterrupt();
                return "bottom right";
            case 0x02:
                clearInterrupt();
                return "dpad middle";
            case 0x04:
                clearInterrupt();
                return "dpad down";
            case 0x08:
                clearInterrupt();
                return "bottom left";
            case 0x10:
                clearInterrupt();
                return "volume down";
            case 0x20:
                clearInterrupt();
                return "dpad left";
            case 0x40:
                clearInterrupt();
                return "volume up";
            case 0x80:
                clearInterrupt();
                return "top left";
        }

        return "";
    }

    void clearInterrupt() {
        // clear interrupt registers
        wiringPiI2CReadReg8(bus, MCP23017_INTCAPA);
        wiringPiI2CReadReg8(bus, MCP23017_INTCAPB);
    }

    void shutdown() {
        // set pins output
        wiringPiI2CWriteReg8(bus, MCP23017_IODIRB, 0x00);

        // set pin power button pin low
        wiringPiI2CWriteReg8(bus, MCP23017_OLATB, 0x00);
    }

 private:
    int bus;

 private:
    void bitWrite(int x, int n, bool b) {
        if (n <= 7 && n >= 0) {
            if (b) {
                x |= (1u << n);
            } else {
                x &= ~(1u << n);
            }
        }
    }
};

#endif  // HARDWARE_ARM_MCP23017_HANDLER_H_
