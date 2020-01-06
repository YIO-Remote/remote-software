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

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QVariant>

#include "config.h"
#include "hardware/buttonhandler.h"
#include "hardware/hardwarefactory.h"
#include "hardware/touchdetect.h"
#include "integrations/integrations.h"
#include "logger.h"
#include "yioapi.h"

class StandbyControl : public QObject {
    Q_OBJECT

 public:
    enum Mode { ON, DIM, STANDBY, WIFI_OFF };
    Q_ENUM(Mode)

    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QString screenOnTime READ screenOnTime NOTIFY screenOnTimeChanged)
    Q_PROPERTY(QString screenOffTime READ screenOffTime NOTIFY screenOffTimeChanged)
    Q_PROPERTY(QVariant batteryData READ batteryData NOTIFY batteryDataChanged)

    int              mode() { return m_mode; }
    Q_INVOKABLE void setMode(int mode);

    Q_INVOKABLE void init();
    Q_INVOKABLE void shutdown();

    Q_INVOKABLE void wakeup();

    QString screenOnTime() { return secondsToHours(m_screenOnTime); }
    QString screenOffTime() { return secondsToHours(m_screenOffTime); }

    QVariant batteryData() { return m_batteryData; }

    explicit StandbyControl(DisplayControl* displayControl, ProximitySensor* proximitySensor, LightSensor* lightSensor,
                            TouchEventFilter* touchEventFilter, InterruptHandler* interruptHandler,
                            ButtonHandler* buttonHandler, WifiControl* wifiControl, BatteryFuelGauge* batteryFuelGauge,
                            Config* config, YioAPI* api, Integrations* integrations, QObject* parent = nullptr);
    virtual ~StandbyControl();

    static StandbyControl* getInstance() { return s_instance; }
    static QObject*        getQMLInstance(QQmlEngine* engine, QJSEngine* scriptEngine);

 signals:
    void modeChanged();
    void standByOn();
    void standByOff();
    void screenOnTimeChanged();
    void screenOffTimeChanged();
    void batteryDataChanged();

 private:
    static StandbyControl* s_instance;

    QLoggingCategory  m_log;
    Config*           m_config;
    YioAPI*           m_api;
    Integrations*     m_integrations;
    DisplayControl*   m_displayControl;
    ProximitySensor*  m_proximitySensor;
    LightSensor*      m_lightsensor;
    TouchEventFilter* m_touchEventFilter;
    InterruptHandler* m_interruptHandler;
    ButtonHandler*    m_buttonHandler;
    WifiControl*      m_wifiControl;
    BatteryFuelGauge* m_batteryFuelGauge;

    int m_mode = ON;

    int m_displayDimTime = 20;  // seconds, not changeable by user
    int m_standByTime    = 30;  // seconds, not changeable by user
    int m_wifiOffTime    = 0;   // seconds, 0 means never, loaded from config.settings
    int m_shutDownTime   = 0;   // seconds, 0 means never, loaded from config.settings

    int m_screenOnTime  = 0;  // seconds
    int m_screenOffTime = 0;  // seconds

    QTimer* m_secondsTimer = new QTimer(this);
    int     m_elapsedTime  = 0;

    void    readAmbientLight();
    int     mapValues(int inValue, int minInRange, int maxInRange, int minOutRange, int maxOutRange);
    QString secondsToHours(int value);

    int     m_batteryCheckElapsedTime = 0;
    int     m_batteryCheckTime        = 600000;
    QTimer* m_shutdownTimer           = new QTimer(this);
    int     m_shutDownDelay           = 20000;

    void         getBatteryData();
    QVariantList m_batteryData;

 private slots:
    void onSecondsTimerTimeout();
    void loadSettings();
    void onTouchDetected();
    void onProximityDetected();
    void onButtonPressDetected(int button);
    void onAveragePowerChanged();
    void onCriticalLowBattery();
};
