#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QThread>
#include <QtDebug>

#ifdef __arm__
#include "apds9960.h"
#endif

class ProximityGestureControl : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(int ambientLight READ getambientLight)
    Q_PROPERTY(int proximity NOTIFY proximityEvent)
    Q_PROPERTY(QString gesture READ getgesture NOTIFY gestureEvent)
    Q_PROPERTY(QString apds9960Error READ getapds9960Error NOTIFY apds9960Notify)
    Q_PROPERTY(int proximitySetting READ getproximitySetting WRITE setproximitySetting)

    int getambientLight() {
        return int(m_ambientLight);
    }

    QString getgesture()
    {
        return m_gesture;
    }

    QString getapds9960Error()
    {
        return m_apds9960Error;
    }

    int getproximitySetting() {
        return m_proximitySetting;
    }

    void setproximitySetting(int proximity) {
        m_proximitySetting = proximity;
    }

    Q_INVOKABLE void proximityDetection(bool state)
    {
        m_proximityDetection = state;
#ifdef __arm__
        if (state) {
            // turn on
            // set the proximity threshold
            apds.setProximityInterruptThreshold(0, uint8_t(m_proximitySetting), 1);

            //set the proximity gain
            apds.setProxGain(APDS9960_PGAIN_2X);

            apds.setLED(APDS9960_LEDDRIVE_50MA, APDS9960_LEDBOOST_200PCNT);

            // enable interrupt
            apds.enableProximityInterrupt();

        } else {
            // turn off
            apds.disableProximityInterrupt();
        }
#endif
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
        while (!apds.colorDataReady())
        {
            delay(5);
        }

        apds.getAmbientLight(m_ambientLight);
        qDebug() << "Lux:" << m_ambientLight;
#endif
        return int(m_ambientLight);
    }

    Q_INVOKABLE void readInterrupt()
    {
#ifdef __arm__
        m_proximity = apds.readProximity();
        qDebug() << "Proximity" << m_proximity;

        if (m_proximityDetection) {

            if (m_proximity > m_proximitySetting) {

                // turn of proximity detection
                qDebug() << "Proximity detected, turning detection off";
                proximityDetection(false);

                // let qml know
                emit proximityEvent();

            } /*else if (m_proximity == 0) {
                // reset interrupt
                qDebug() << "Turning proximity interrupt on-off";
//                proximityDetection(false);
                //                apds.enableProximity(false);
                //                delay(100);
                //                apds.enableProximity(true);
//                proximityDetection(true);
            }*/
        }

        // clear the interrupt
        apds.clearInterrupt();
#endif
    }

public:
    ProximityGestureControl()
    {
#ifdef __arm__
        // intialize the sensor
        if (!apds.begin()) {
            qDebug() << "Cannot initialise the APDS9960 sensor";
            //: Error message that shows up as notification when light value cannot be read
            m_apds9960Error = tr("Cannot initialize the proximity sensor. Please restart the remote.");
            emit apds9960Notify();
            return;
        }

        delay(100);

        // turn on the light sensor
        apds.enableColor(true);

        // turn on proximity sensor
        apds.enableProximity(true);

        // read ambient light
        readAmbientLight();
#endif
    }

    ~ProximityGestureControl()
    {

    }

signals:
    void proximityEvent();
    void gestureEvent();
    void apds9960Notify();

private:

#ifdef __arm__
    APDS9960 apds = APDS9960();
#endif
    uint16_t m_ambientLight;
    uint8_t m_proximity;
    QString m_gesture;
    QString m_apds9960Error;
    bool m_proximityDetection = false;
    bool m_gestureDetection = false;
    int m_proximitySetting = 70; // default value
};

#endif // PROXIMITY_GESTURE_CONTROL_H
