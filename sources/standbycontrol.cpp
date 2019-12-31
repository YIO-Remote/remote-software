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

#include "standbycontrol.h"

#include "integrations/integration.h"

StandbyControl *StandbyControl::s_instance = nullptr;

void StandbyControl::setMode(int mode) {
    m_mode = mode;
    if (m_mode == ON) {
        emit standByOff();
        m_elapsedTime = 0;
    }
    if (m_mode == STANDBY) {
        emit standByOn();
    }
    emit modeChanged();
}

StandbyControl::StandbyControl(QObject *parent) : QObject(parent), m_log("STANDBYCONTROL") {
    s_instance = this;

    // define logging category
    Logger::getInstance()->defineLogCategory(m_log.categoryName(), QtMsgType::QtDebugMsg, &m_log);

    // load configuration

    // start timer that counts every seconds
    m_secondsTimer->setInterval(1000);
    // connect to timer signal
    connect(m_secondsTimer, &QTimer::timeout, this, &StandbyControl::onSecondsTimerTimeout);
    m_secondsTimer->start();

    // connect to signals of hardware devices
}

StandbyControl::~StandbyControl() { s_instance = nullptr; }

void StandbyControl::onSecondsTimerTimeout() {
    // increase the elapsed time
    m_elapsedTime++;

    // if it's on, then inscrease screen on time
    if (m_mode == ON || m_mode == DIM) m_screenOnTime++;

    // if it's standby, then inscrease screen off time
    if (m_mode == STANDBY || m_mode == WIFI_OFF) m_screenOffTime++;

    // STATE CONTROL
    // DIM
    if (m_elapsedTime == m_displayDimTime && m_mode == ON) {
        // dim the display

        // change mode
        setMode(DIM);
    }

    // STANDBY
    if (m_elapsedTime == m_standByTime && m_mode == DIM) {
        // turn on proximity detection

        // turn off backlight

        // put the display to standby mode

        // stop bluetooth scanning

        // reset battery charging screen

        setMode(STANDBY);
    }

    // TURN OFF BLUETOOTH
    if (m_elapsedTime == m_standByTime + 20 && m_mode == STANDBY &&
        m_config->getSettings().value("bluetootharea").toBool()) {
        // turn off bluetooth
    }

    // TURN OFF WIFI
    if (m_elapsedTime == m_wifiOffTime && m_wifiOffTime != 0 && m_mode == STANDBY /*&& battery_averagepower <= 0*/) {
        // disconnect integrations
        for (int i = 0; i < m_integrations->list().length(); i++) {
            Integration *integrationObj = qobject_cast<Integration *>(m_integrations->list().at(i));
            integrationObj->disconnect();
        }

        // turn off API
        m_api->stop();

        // turn off wifi

        setMode(WIFI_OFF);
    }

    // SHUTDOWN
    if (m_elapsedTime == m_shutDownTime && m_shutDownTime != 0 &&
        (m_mode == STANDBY || m_mode == WIFI_OFF) /*&& battery_averagepower <= 0*/) {
        qCInfo(m_log) << "TIMER SHUTDOWN";

        // show closing screen
        QObject *loadingScreen = m_config->getQMLObject("");
        loadingScreen->setProperty("source", "qrc:/basic_ui/ClosingScreen.qml");
        loadingScreen->setProperty("active", true);
    }
}
