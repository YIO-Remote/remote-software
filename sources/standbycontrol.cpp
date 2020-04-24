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

#include <QLoggingCategory>
#include <QtDebug>

#include "yio-interface/integrationinterface.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "standbycontrol");

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

void StandbyControl::init() {
    m_secondsTimer->start();
    m_batteryFuelGauge->begin();
}

void StandbyControl::shutdown() { m_interruptHandler->shutdown(); }

StandbyControl::StandbyControl(DisplayControl *displayControl, ProximitySensor *proximitySensor,
                               LightSensor *lightSensor, TouchEventFilter *touchEventFilter,
                               InterruptHandler *interruptHandler, ButtonHandler *buttonHandler,
                               WifiControl *wifiControl, BatteryFuelGauge *batteryFuelGauge, Config *config,
                               YioAPI *api, Integrations *integrations, QObject *parent)
    : QObject(parent),
      m_config(config),
      m_api(api),
      m_integrations(integrations),
      m_displayControl(displayControl),
      m_proximitySensor(proximitySensor),
      m_lightsensor(lightSensor),
      m_touchEventFilter(touchEventFilter),
      m_interruptHandler(interruptHandler),
      m_buttonHandler(buttonHandler),
      m_wifiControl(wifiControl),
      m_batteryFuelGauge(batteryFuelGauge) {
    s_instance = this;

    // load configuration
    loadSettings();
    // connect to config change signals
    connect(m_config, &Config::settingsChanged, this, &StandbyControl::loadSettings);

    // start timer that counts every seconds
    m_secondsTimer->setInterval(1000);
    // connect to timer signal
    connect(m_secondsTimer, &QTimer::timeout, this, &StandbyControl::onSecondsTimerTimeout);
    //    m_secondsTimer->start();

    // connect to signals of hardware devices
    connect(m_touchEventFilter, &TouchEventFilter::detectedChanged, this, &StandbyControl::onTouchDetected);
    connect(m_proximitySensor, &ProximitySensor::proximityEvent, this, &StandbyControl::onProximityDetected);
    connect(m_buttonHandler, &ButtonHandler::buttonPressed, this, &StandbyControl::onButtonPressDetected);

    // connect to signals of the battery fuel gauge
    connect(m_batteryFuelGauge, &BatteryFuelGauge::criticalLowBattery, this, &StandbyControl::onCriticalLowBattery);
    connect(m_batteryFuelGauge, &BatteryFuelGauge::averagePowerChanged, this, &StandbyControl::onAveragePowerChanged);

    // setting up shutdown delay
    m_shutdownTimer->setSingleShot(true);
    m_shutdownTimer->setInterval(m_shutDownDelay);
    connect(m_shutdownTimer, &QTimer::timeout, this, [&]() {
        // show closing screen
        QObject *loadingScreen = m_config->getQMLObject("loadingScreen");
        loadingScreen->setProperty("source", "qrc:/basic_ui/ClosingScreen.qml");
        loadingScreen->setProperty("active", true);
    });

    qCDebug(CLASS_LC) << "Standby Control intialized";
}

StandbyControl::~StandbyControl() { s_instance = nullptr; }

QObject *StandbyControl::getQMLInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance;
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

void StandbyControl::wakeup() {
    switch (mode()) {
        case (DIM): {
            qCDebug(CLASS_LC) << "Wakeup from DIM";
            readAmbientLight();
            setMode(ON);
        } break;

        case (STANDBY): {
            qCDebug(CLASS_LC) << "Wakeup from STANDBY";

            m_format.setSwapInterval(1);
            QSurfaceFormat::setDefaultFormat(m_format);
            qCDebug(CLASS_LC) << "Changing swap interval to " << m_format.swapInterval();

            // delay reading ambient light so the sensor is clear of your hand
            QTimer *timer = new QTimer(this);
            timer->setSingleShot(true);

            QObject *context = new QObject(this);

            connect(timer, &QTimer::timeout, context, [=]() {
                m_displayControl->setMode(DisplayControl::StandbyOff);
                readAmbientLight();
                timer->deleteLater();
                context->deleteLater();
            });

            timer->start(300);

            // integrations out of standby mode
            for (int i = 0; i < m_integrations->list().length(); i++) {
                IntegrationInterface *integrationObj =
                    qobject_cast<IntegrationInterface *>(m_integrations->list().at(i));
                integrationObj->leaveStandby();
            }

            // start bluetooth scanning

            // reset battery charging screen
            QObject *showClock = m_config->getQMLObject("showClock");
            QMetaObject::invokeMethod(showClock, "start", Qt::AutoConnection);

            QTimer::singleShot(300, this, [=]() {
                // disable touch event catcher
                QObject *touchEventCatcher = m_config->getQMLObject("touchEventCatcher");
                touchEventCatcher->setProperty("enabled", false);
            });

            setMode(ON);
        } break;

        case (WIFI_OFF): {
            qCDebug(CLASS_LC) << "Wakeup from WIFI_OFF";

            m_format.setSwapInterval(1);
            QSurfaceFormat::setDefaultFormat(m_format);
            qCDebug(CLASS_LC) << "Changing swap interval to " << m_format.swapInterval();

            m_wifiControl->on();

            m_displayControl->setMode(DisplayControl::StandbyOff);
            readAmbientLight();

            // connect integrations
            for (int i = 0; i < m_integrations->list().length(); i++) {
                IntegrationInterface *integrationObj =
                    qobject_cast<IntegrationInterface *>(m_integrations->list().at(i));
                integrationObj->connect();
            }

            m_api->start();

            // start bluetooth scanning

            // reset battery charging screen
            QObject *showClock = m_config->getQMLObject("showClock");
            QMetaObject::invokeMethod(showClock, "start", Qt::AutoConnection);

            QTimer::singleShot(300, this, [=]() {
                // disable touch event catcher
                QObject *touchEventCatcher = m_config->getQMLObject("touchEventCatcher");
                touchEventCatcher->setProperty("enabled", false);
            });

            setMode(ON);
        } break;
    }

    m_elapsedTime = 0;
}

void StandbyControl::readAmbientLight() {
    int lux = m_lightsensor->readAmbientLight();
    m_displayControl->setAmbientBrightness(mapValues(lux, 0, 40, 15, 100));

    if (m_config->getSettings().value("autobrightness").toBool()) {
        m_displayControl->setBrightness(m_displayControl->ambientBrightness());
    } else {
        m_displayControl->setBrightness(m_displayControl->userBrightness());
    }
}

int StandbyControl::mapValues(int inValue, int minInRange, int maxInRange, int minOutRange, int maxOutRange) {
    int leftSpan    = maxInRange - minInRange;
    int rightSpan   = maxOutRange - minOutRange;
    int valueScaled = (inValue - minInRange) / leftSpan;
    return (minOutRange + (valueScaled * rightSpan));
}

QString StandbyControl::secondsToHours(int value) {
    QString returnString;

    int hour = value / 3600;
    if (hour != 0) {
        returnString.append(QString::number(hour)).append(QString("h "));
    }
    value %= 3600;
    int minutes = value / 60;
    if (minutes != 0) {
        returnString.append(QString::number(minutes)).append(QString("m "));
    }
    value %= 60;
    returnString.append(QString::number(value)).append(QString("s "));

    return returnString;
}

void StandbyControl::getBatteryData() {
    if (m_batteryData.length() == 36 && !m_batteryData.isEmpty()) {
        m_batteryData.removeFirst();
    }

    QVariantMap map;
    map.insert("timestamp", QDateTime::currentDateTime());
    map.insert("level", m_batteryFuelGauge->getLevel());
    map.insert("power", m_batteryFuelGauge->getAveragePower());
    map.insert("voltage", m_batteryFuelGauge->getVoltage());

    m_batteryData.append(map);
    emit batteryDataChanged();
}

void StandbyControl::onSecondsTimerTimeout() {
    // increase the elapsed time
    m_elapsedTime++;

    // increase battery data logger time
    m_batteryCheckElapsedTime++;

    if (m_batteryCheckElapsedTime == m_batteryCheckTime) {
        m_batteryCheckElapsedTime = 0;
        getBatteryData();
    }

    // if it's on, then inscrease screen on time
    if (m_mode == ON || m_mode == DIM) {
        m_screenOnTime++;
        emit screenOnTimeChanged();
    }

    // if it's standby, then inscrease screen off time
    if (m_mode == STANDBY || m_mode == WIFI_OFF) {
        m_screenOffTime++;
        emit screenOffTimeChanged();
    }

    // STATE CONTROL
    // DIM
    if (m_elapsedTime == m_displayDimTime && m_mode == ON) {
        // dim the display
        m_displayControl->setBrightness(10);

        // change mode
        setMode(DIM);
        qCDebug(CLASS_LC) << "State set to DIM";
    }

    // STANDBY
    if (m_elapsedTime == m_standByTime && m_mode == DIM) {
        // turn on proximity detection
        m_proximitySensor->proximityDetection(true);

        // turn off backlight
        m_displayControl->setBrightness(0);

        // put the display to standby mode
        m_displayControl->setMode(DisplayControl::StandbyOn);

        // TODO(martonborzak):
        // stop bluetooth scanning

        // reset battery charging screen
        QObject *resetClock = m_config->getQMLObject("resetClock");
        QMetaObject::invokeMethod(resetClock, "start", Qt::AutoConnection);

        // enable touch event catcher
        QObject *touchEventCatcher = m_config->getQMLObject("touchEventCatcher");
        touchEventCatcher->setProperty("enabled", true);

        setMode(STANDBY);

        // integrations set standby mode
        for (int i = 0; i < m_integrations->list().length(); i++) {
            IntegrationInterface *integrationObj = qobject_cast<IntegrationInterface *>(m_integrations->list().at(i));
            integrationObj->enterStandby();
        }

        m_format.setSwapInterval(60);
        QSurfaceFormat::setDefaultFormat(m_format);
        qCDebug(CLASS_LC) << "Changing swap interval to " << m_format.swapInterval();

        qCDebug(CLASS_LC) << "State set to STANDBY";
    }

    // TURN OFF BLUETOOTH
    if (m_elapsedTime == m_standByTime + 20 && m_mode == STANDBY &&
        m_config->getSettings().value("bluetootharea").toBool()) {
        // TODO(martonborzak):
        // turn off bluetooth
    }

    // TURN OFF WIFI
    if (m_elapsedTime == m_wifiOffTime && m_wifiOffTime != 0 && m_mode == STANDBY &&
        m_batteryFuelGauge->getAveragePower() <= 0) {
        // disconnect integrations
        for (int i = 0; i < m_integrations->list().length(); i++) {
            IntegrationInterface *integrationObj = qobject_cast<IntegrationInterface *>(m_integrations->list().at(i));
            integrationObj->disconnect();
        }

        // turn off API
        m_api->stop();

        // turn off wifi
        m_wifiControl->off();

        setMode(WIFI_OFF);
        qCDebug(CLASS_LC) << "State set to WIFI OFF";
    }

    // SHUTDOWN
    if (m_elapsedTime == m_shutDownTime && m_shutDownTime != 0 && (m_mode == STANDBY || m_mode == WIFI_OFF) &&
        m_batteryFuelGauge->getAveragePower() < 0 && !m_batteryFuelGauge->getIsCharging()) {
        qCInfo(CLASS_LC) << "TIMER SHUTDOWN"
                         << "Average power:" << m_batteryFuelGauge->getAveragePower()
                         << "Is charging:" << m_batteryFuelGauge->getIsCharging();

        // show closing screen
        QObject *loadingScreen = m_config->getQMLObject("loadingScreen");
        loadingScreen->setProperty("source", "qrc:/basic_ui/ClosingScreen.qml");
        loadingScreen->setProperty("active", true);
    }
}

void StandbyControl::loadSettings() {
    QVariantMap settings = m_config->getSettings();
    m_wifiOffTime        = settings.value("wifitime").toInt();
    m_shutDownTime       = settings.value("shutdowntime").toInt();
}

void StandbyControl::onTouchDetected() {
    wakeup();
    m_proximitySensor->proximityDetection(false);
}

void StandbyControl::onProximityDetected() {
    qCDebug(CLASS_LC) << "Proximity detected";
    wakeup();
}

void StandbyControl::onButtonPressDetected(int button) {
    Q_UNUSED(button)
    wakeup();
    m_proximitySensor->proximityDetection(false);
}

void StandbyControl::onAveragePowerChanged() {
    // if the remote was put on charger, stop the shutdown timer
    if (m_batteryFuelGauge->getAveragePower() >= 0 && m_shutdownTimer->isActive()) {
        m_shutdownTimer->stop();
    }
}

void StandbyControl::onCriticalLowBattery() { m_shutdownTimer->start(); }
