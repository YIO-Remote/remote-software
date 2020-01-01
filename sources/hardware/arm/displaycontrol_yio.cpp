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

#include <QFuture>
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrentRun>

#include <mcp23017.h>
#include <wiringPi.h>

#include "displaycontrol_yio.h"
#include "mcp23017_handler.h"

#define CLK 107
#define MOSI 106
#define CS 105
#define RST 104

DisplayControlYio::DisplayControlYio(QObject *parent) : DisplayControl(parent) { setup(); }

void DisplayControlYio::setup() {
    wiringPiSetup();
    mcp23017Setup(100, 0x21);
}

bool DisplayControlYio::setmode(const QString &mode) {
    if (mode == "standbyon") {
        QFuture<void> future = QtConcurrent::run([&]() {
            delay(400);  // wait until dimming of the display is done
            spi_screenreg_set(0x10, 0xffff, 0xffff);
            delay(120);
            spi_screenreg_set(0x28, 0xffff, 0xffff);
        });
        return true;
    }
    if (mode == "standbyoff") {
        QFuture<void> future = QtConcurrent::run([&]() {
            spi_screenreg_set(0x29, 0xffff, 0xffff);
            spi_screenreg_set(0x11, 0xffff, 0xffff);
        });
        return true;
    }
    return false;
}

void DisplayControlYio::setBrightness(int from, int to) {
    QFuture<void> future = QtConcurrent::run(
        [&](int from, int to) {
            if (from == 0 && digitalRead(26) == 0) {
                pinMode(26, PWM_OUTPUT);
                pwmSetMode(PWM_MODE_MS);
                pwmSetClock(1000);
                pwmSetRange(100);
            }

            if (from >= to) {
                // dim down
                for (int i = from; i > to - 1; i--) {
                    pwmWrite(26, i);
                    delay(10);
                    if (i == 0) {
                        delay(100);
                        pinMode(26, OUTPUT);
                        digitalWrite(26, 0);
                    }
                }
            } else {
                // dim up
                for (int i = from; i < to + 1; i++) {
                    pwmWrite(26, i);
                    delay(10);
                }
            }
        },
        from, to);
}

void DisplayControlYio::spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1) {
    int32_t i;
    int32_t control_bit;

    pinMode(MOSI, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(CS, OUTPUT);
    nanosleep(&ts, NULL);
    nanosleep(&ts, NULL);

    digitalWrite(CS, HIGH);
    digitalWrite(MOSI, HIGH);
    digitalWrite(CLK, LOW);
    nanosleep(&ts2, NULL);

    digitalWrite(CS, LOW);
    control_bit = 0x0000;
    Addr = (control_bit | Addr);

    for (i = 0; i < 9; i++) {
        if (Addr & (1 << (8 - i))) {
            digitalWrite(MOSI, HIGH);
        } else {
            digitalWrite(MOSI, LOW);
        }

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

    if (0xffff == Data0) {
        return;
    }

    digitalWrite(CS, LOW);

    control_bit = 0x0100;
    Data0 = (control_bit | Data0);

    // data
    for (i = 0; i < 9; i++) {
        if (Data0 & (1 << (8 - i))) {
            digitalWrite(MOSI, HIGH);
        } else {
            digitalWrite(MOSI, LOW);
        }
        digitalWrite(CLK, HIGH);
        nanosleep(&ts, NULL);
        digitalWrite(CLK, LOW);
        nanosleep(&ts, NULL);
    }

    digitalWrite(CS, HIGH);
    digitalWrite(CLK, LOW);
    digitalWrite(MOSI, LOW);
    nanosleep(&ts2, NULL);

    if (0xffff == Data1) {
        return;
    }

    digitalWrite(CS, LOW);

    control_bit = 0x0100;
    Data1 = (control_bit | Data1);

    // data
    for (i = 0; i < 9; i++) {
        if (Data1 & (1 << (8 - i))) {
            digitalWrite(MOSI, HIGH);
        } else {
            digitalWrite(MOSI, LOW);
        }
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
