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

#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QtDebug>

#ifdef __arm__
#include "arm/apds9960.h"
#endif

#include "../notifications.h"

class ProximityGestureControl : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(int          ambientLight        READ getambientLight)
    Q_PROPERTY(int          proximity           READ proximity                  NOTIFY proximityEvent)
    Q_PROPERTY(QString      gesture             READ getgesture                 NOTIFY gestureEvent)
    Q_PROPERTY(int          proximitySetting    READ getproximitySetting        WRITE setproximitySetting)

    int getambientLight() { return int(m_ambientLight); }

    QString getgesture() { return m_gesture; }

    int getproximitySetting() { return m_proximitySetting; }

    void setproximitySetting(int proximity) { m_proximitySetting = proximity; }

    int proximity() { return m_proximity; }

    Q_INVOKABLE void proximityDetection(bool state)
    {
#ifdef __arm__
        if (m_init_success) {
            if (state != m_proximityDetection) {

                if (state) {
                    // turn on
                    apds.enableProximityInterrupt();

                } else {
                    // turn off
                    apds.disableProximityInterrupt();
                }
            }
        }
#endif
        m_proximityDetection = state;
    }

    Q_INVOKABLE void gestureDetection(bool state)
    {
        m_gestureDetection = state;
#ifdef __arm__
        if (state) {
            // turn on

        } else {
            // turn off
        }
#endif
    }

    Q_INVOKABLE int readAmbientLight()
    {
#ifdef __arm__
        if (m_init_success) {
            while (!apds.colorDataReady())
            {
                delay(5);
            }

            apds.getAmbientLight(m_ambientLight);
            qDebug() << "Lux:" << m_ambientLight;
        }
#endif
        return int(m_ambientLight);
    }

    Q_INVOKABLE void readInterrupt()
    {
#ifdef __arm__
        if (m_init_success) {
            m_proximity = apds.readProximity();
            if (m_proximity > 0) {
                // prevent log flooding while docking
                qDebug() << "Proximity" << m_proximity;
            }

            if (m_proximityDetection) {

                if (m_proximity > m_proximitySetting) {

                    // turn of proximity detection
                    qDebug() << "Proximity detected, turning detection off";
                    proximityDetection(false);

                    // let qml know
                    emit proximityEvent();

                }
            }

            // clear the interrupt
            apds.clearInterrupt();
        }
#endif
    }

public:
    ProximityGestureControl()
    {
#ifdef __arm__
        // intialize the sensor
        if (!apds.begin()) {
            qCritical() << "Cannot initialise the APDS9960 sensor";
            //: Error message that shows up as notification when light value cannot be read
//            m_apds9960Error = tr("Cannot initialize the proximity sensor. Please restart the remote.");
            m_init_success = false;
//            emit apds9960Notify();
            Notifications::getInstance()->add(true,tr("Cannot initialize the proximity sensor. Please restart the remote."));
            return;
        }

        m_init_success = true;

        delay(100);

        // turn on the light sensor
        apds.enableColor(true);

        // turn on proximity sensor
        apds.enableProximity(true);

        // set the proximity threshold
        apds.setProximityInterruptThreshold(0, uint8_t(m_proximitySetting), 1);

        //set the proximity gain
        apds.setProxGain(APDS9960_PGAIN_2X);

        //            apds.setLED(APDS9960_LEDDRIVE_100MA, APDS9960_LEDBOOST_200PCNT);
        apds.setLED(APDS9960_LEDDRIVE_25MA, APDS9960_LEDBOOST_100PCNT);

        // read ambient light
        while (!apds.colorDataReady())
        {
            delay(5);
        }

        readAmbientLight();
#endif
    }

    ~ProximityGestureControl() {}

signals:
    void proximityEvent();
    void gestureEvent();

private:

#ifdef __arm__
    APDS9960 apds = APDS9960();
#endif
    bool m_init_success = false;
    uint16_t m_ambientLight = 100;
    uint8_t m_proximity;
    QString m_gesture;
    bool m_proximityDetection = false;
    bool m_gestureDetection = false;
    int m_proximitySetting = 70; // default value
};

#endif // PROXIMITY_GESTURE_CONTROL_H
