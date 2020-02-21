/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#include "displaycontrol_yio.h"

#include <wiringPi.h>

#include <QFuture>
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrentRun>
#include <QtDebug>

#include "mcp23017_handler.h"

#define CLK 107
#define MOSI 106
#define CS 105
#define RST 104

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.display");

DisplayControlYio::DisplayControlYio(int pin, QObject *parent)
    : DisplayControl("YIO display control", parent) {
    Q_ASSERT(pin);
    qCDebug(CLASS_LC()) << name() << "on GPIO" << pin;

    // move the low level hardware handling to a separate thread
    m_thread = new QThread(this);
    DisplayControlYioThread *dcyt = new DisplayControlYioThread(pin);

    connect(this, &DisplayControlYio::enterStandby, dcyt, &DisplayControlYioThread::enterStandby);
    connect(this, &DisplayControlYio::leaveStandby, dcyt, &DisplayControlYioThread::leaveStandby);
    connect(this, &DisplayControlYio::setBrightnessSignal, dcyt, &DisplayControlYioThread::setBrightness);

    dcyt->moveToThread(m_thread);
}

DisplayControlYio::~DisplayControlYio() { close(); }

bool DisplayControlYio::open() {
    if (isOpen()) {
        qCWarning(CLASS_LC) << DBG_WARN_DEVICE_OPEN;
        return true;
    }

    m_thread->start();

    return Device::open();
}

void DisplayControlYio::close() {
    Device::close();

    if (m_thread->isRunning()) {
        m_thread->exit();
        m_thread->wait(3000);
        m_thread->deleteLater();
    }
}

bool DisplayControlYio::setMode(Mode mode) {
    if (!isOpen()) {
        qCWarning(CLASS_LC()) << "Display device is closed. Cannot change mode to" << mode;
        return false;
    }

    if (mode == StandbyOn) {
        qCDebug(CLASS_LC) << "Entering standby";
        emit enterStandby();
        return true;
    } else if (mode == StandbyOff) {
        qCDebug(CLASS_LC) << "Leaving standby";
        emit leaveStandby();
        return true;
    }

    return false;
}

void DisplayControlYio::setBrightness(int from, int to) {
    if (!isOpen()) {
        qCWarning(CLASS_LC()) << "Display device is closed. Cannot change brightness";
        return;
    }
    emit setBrightnessSignal(from, to);

    m_currentBrightness = to;
    emit currentBrightnessChanged();
}

void DisplayControlYio::setBrightness(int to) { setBrightness(m_currentBrightness, to); }

const QLoggingCategory &DisplayControlYio::logCategory() const { return CLASS_LC(); }

// THREADED STUFF

void DisplayControlYioThread::setBrightness(int from, int to) {
    qCDebug(CLASS_LC) << "Changing brightness:" << from << " -> " << to;

    if (from < 0) {
        from = 0;
    }
    if (from > 100) {
        from = 100;
    }
    if (to < 0) {
        to = 0;
    }
    if (to > 100) {
        to = 100;
    }

    if (from == 0 && digitalRead(m_pin) == 0) {
        pinMode(m_pin, PWM_OUTPUT);
        pwmSetMode(PWM_MODE_MS);
        pwmSetClock(1000);
        pwmSetRange(100);
    }

    if (from == 0) {
        delay(300);
    }

    if (from >= to) {
        // dim down
        for (int i = from; i > to - 1; i--) {
            pwmWrite(m_pin, i);
            delay(10);
            if (i == 0) {
                delay(100);
                pinMode(m_pin, OUTPUT);
                digitalWrite(m_pin, 0);
            }
        }
    } else {
        // dim up
        for (int i = from; i < to + 1; i++) {
            pwmWrite(m_pin, i);
            delay(10);
        }
    }
}

void DisplayControlYioThread::spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1) {
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

void DisplayControlYioThread::enterStandby() {
    delay(400);  // wait until dimming of the display is done
    spi_screenreg_set(0x10, 0xffff, 0xffff);
    delay(120);
    spi_screenreg_set(0x28, 0xffff, 0xffff);
}

void DisplayControlYioThread::leaveStandby() {
    spi_screenreg_set(0x29, 0xffff, 0xffff);
    spi_screenreg_set(0x11, 0xffff, 0xffff);
}
