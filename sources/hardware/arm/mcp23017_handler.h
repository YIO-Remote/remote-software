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

#pragma once

#include <QLoggingCategory>
#include <QString>
#include <QtDebug>

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "../interrupthandler.h"

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

static Q_LOGGING_CATEGORY(CLASS_LC2, "MCP23017");

class MCP23017 {
 public:
    MCP23017() : m_i2cFd(0) {}

    ~MCP23017() {
        if (m_i2cFd > 0) {
            ::close(m_i2cFd);
        }
    }

    bool setup(const QString &i2cDevice, int i2cDeviceId) {
        if (m_i2cFd > 0) {
            return true;
        }

        m_i2cFd = wiringPiI2CSetupInterface(qPrintable(i2cDevice), i2cDeviceId);
        if (m_i2cFd == -1) {
            qCCritical(CLASS_LC2) << "Unable to open or select I2C device" << i2cDevice << "on" << i2cDeviceId;
            return false;
        }

        // set up all inputs on both ports
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_IODIRA, 0xbf);  // 0xbf 0b10111111
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_IODIRB, 0xff);

        // set up interrupts
        int ioconfValue = wiringPiI2CReadReg8(m_i2cFd, MCP23017_IOCONA);
        bitWrite(ioconfValue, 6, true);
        bitWrite(ioconfValue, 2, false);
        bitWrite(ioconfValue, 1, false);
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_IOCONA, ioconfValue);

        ioconfValue = wiringPiI2CReadReg8(m_i2cFd, MCP23017_IOCONB);
        bitWrite(ioconfValue, 6, true);   // mirror
        bitWrite(ioconfValue, 2, false);  //
        bitWrite(ioconfValue, 1, false);  // polarity
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_IOCONB, ioconfValue);

        // setup pin for interrupt
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_INTCONA, 0x00);
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_GPPUA, 0xbf);

        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_INTCONB, 0x00);
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_GPPUB, 0xff);

        // enable pin for interrupt
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_GPINTENA, 0xbf);
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_GPINTENB, 0xff);

        wiringPiI2CReadReg8(m_i2cFd, MCP23017_INTCAPA);
        wiringPiI2CReadReg8(m_i2cFd, MCP23017_INTCAPB);

        return true;
    }

    int readInterrupt() {
        int intf;

        intf = wiringPiI2CReadReg8(m_i2cFd, MCP23017_INTFA);

        switch (intf) {
            case 0x01:
                clearInterrupt();
                return InterruptHandler::APDS9960;
            case 0x02:
                clearInterrupt();
                return InterruptHandler::DPAD_UP;
            case 0x04:
                clearInterrupt();
                return InterruptHandler::TOP_RIGHT;
            case 0x08:
                clearInterrupt();
                return InterruptHandler::CHANNEL_UP;
            case 0x10:
                clearInterrupt();
                return InterruptHandler::DPAD_RIGHT;
            case 0x20:
                clearInterrupt();
                return InterruptHandler::CHANNEL_DOWN;
            case 0x80:
                clearInterrupt();
                return InterruptHandler::BATTERY;
        }

        intf = wiringPiI2CReadReg8(m_i2cFd, MCP23017_INTFB);

        switch (intf) {
            case 0x01:
                clearInterrupt();
                return InterruptHandler::BOTTOM_RIGHT;
            case 0x02:
                clearInterrupt();
                return InterruptHandler::DPAD_MIDDLE;
            case 0x04:
                clearInterrupt();
                return InterruptHandler::DPAD_DOWN;
            case 0x08:
                clearInterrupt();
                return InterruptHandler::BOTTOM_LEFT;
            case 0x10:
                clearInterrupt();
                return InterruptHandler::VOLUME_DOWN;
            case 0x20:
                clearInterrupt();
                return InterruptHandler::DPAD_LEFT;
            case 0x40:
                clearInterrupt();
                return InterruptHandler::VOLUME_UP;
            case 0x80:
                clearInterrupt();
                return InterruptHandler::TOP_LEFT;
        }
        return -1;
    }

    void clearInterrupt() {
        // clear interrupt registers
        wiringPiI2CReadReg8(m_i2cFd, MCP23017_INTCAPA);
        wiringPiI2CReadReg8(m_i2cFd, MCP23017_INTCAPB);
    }

    void shutdown() {
        // set pins output
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_IODIRB, 0x00);

        // set pin power button pin low
        wiringPiI2CWriteReg8(m_i2cFd, MCP23017_OLATB, 0x00);
    }

 private:
    int m_i2cFd;

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
