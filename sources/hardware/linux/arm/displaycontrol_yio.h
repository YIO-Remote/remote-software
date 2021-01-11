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

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <QScreen>
#include <QThread>

#include "../../displaycontrol.h"

class DisplayControlYio : public DisplayControl {
    Q_OBJECT

 public:
    explicit DisplayControlYio(int backlightPin, QObject* parent = nullptr);
    ~DisplayControlYio() override;

    bool setMode(Mode mode) override;
    void setBrightness(int from, int to) override;
    void setBrightness(int to) override;

    int  currentBrightness() override { return m_currentBrightness; }
    int  ambientBrightness() override { return m_ambientBrightness; }
    int  userBrightness() override { return m_userBrightness; }
    void setAmbientBrightness(int value) override { m_ambientBrightness = value; }
    void setUserBrightness(int value) override { m_userBrightness = value; }

    int   width() override { return 480; }
    int   height() override { return 800; }
    qreal pixelDensity() override;

 signals:
    void enterStandby();
    void leaveStandby();
    void setBrightnessSignal(int from, int to);

    // Device interface
 public:
    bool open() override;
    void close() override;

 protected:
    const QLoggingCategory& logCategory() const override;

 private:
    int m_currentBrightness = 100;
    int m_ambientBrightness = 100;
    int m_userBrightness    = 100;

    QThread* m_thread;
};

class DisplayControlYioThread : public QObject {
    Q_OBJECT

    // define timing
    struct timespec ts  = {0, 40L};
    struct timespec ts2 = {0, 100L};
    struct timespec ts3 = {0, 300L};

 public:
    explicit DisplayControlYioThread(int backlightPin) : m_backlightPin(backlightPin) {}
    virtual ~DisplayControlYioThread() {}

 private:
    void spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1);

 public slots:
    void setBrightness(int from, int to);
    void enterStandby();
    void leaveStandby();

 private:
    // GPIO pin
    int m_backlightPin;
};
