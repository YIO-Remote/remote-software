#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QtDebug>
#include <QThread>

#include "apds9960.h"

class ProximityGestureControl : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(int ambientLight READ getambientLight)
    Q_PROPERTY(int proximity NOTIFY proximityEvent)
    Q_PROPERTY(QString gesture READ getgesture NOTIFY gestureEvent)
    Q_PROPERTY(QString apds9960Error READ getapds9960Error NOTIFY apds9960Notify)

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

    Q_INVOKABLE void proximityDetection(bool state)
    {
        m_proximityDetection = state;

        if (state) {
            // turn on
            apds.setProximityGain(2);
            apds.setProximityIntLowThreshold(0);
            apds.setProximityIntHighThreshold(150);
            apds.clearProximityInt();
            apds.setLEDBoost(0);
            apds.enableProximitySensor(true);

        } else {
            // turn off
            apds.disableProximitySensor();
        }
    }

    Q_INVOKABLE void gestureDetection(bool state)
    {
        m_gestureDetection = state;

        if (state) {
            // turn on
            apds.setGestureGain(0);
            apds.setLEDBoost(0);
            apds.enableGestureSensor(true);

        } else {
            // turn off
            apds.disableGestureSensor();
        }
    }

    Q_INVOKABLE void readInterrupt() {

        if (m_proximityDetection) {

            // clear the interrupt
            apds.readProximity(m_proximity);
            qDebug() << "Proximity reading:" << m_proximity;
            apds.clearProximityInt();
            apds.enableLightSensor(false);
            delay(200);

            // read the ambient light
            apds.readAmbientLight(m_ambientLight);

            // let qml know
            emit proximityEvent();

            // turn off proximity detection
            proximityDetection(false);

            //            delay(1000);

            // enable gesture detection
            //            gestureDetection(true);

        } else if (m_gestureDetection) {

            // read the gesture
            if ( false == apds.isGestureAvailable() )
            {
                return;
            }

            switch ( apds.readGesture() )
            {
            case DIR_UP:
                m_gesture = "right";
                emit gestureEvent();
                break;
            case DIR_DOWN:
                emit gestureEvent();
                m_gesture = "left";
                break;
            case DIR_LEFT:
                emit gestureEvent();
                m_gesture = "up";
                break;
            case DIR_RIGHT:
                emit gestureEvent();
                m_gesture = "down";
                break;
            }
        }
    }

public:
    ProximityGestureControl()
    {
        if ( (false == apds.init()) || (false == apds.enableLightSensor(false)) )
        {
            qDebug() << "Cannot initialize the proximity sensor";
            m_apds9960Error = "Cannot initialize the proximity sensor";
            emit apds9960Notify();
        }

        delay(500);

        // read the ambient light when it's on first
        if ( !apds.readAmbientLight(m_ambientLight)) {
            m_apds9960Error = "Error reading light values";
        } else {
            apds.disableLightSensor();
            delay(200);
            //            gestureDetection(true);
        }
    }

    ~ProximityGestureControl()
    {

    }

signals:
    void proximityEvent();
    void gestureEvent();
    void apds9960Notify();

private:
    APDS9960 apds = APDS9960();
    uint16_t m_ambientLight;
    uint8_t m_proximity;
    QString m_gesture;
    QString m_apds9960Error;
    bool m_proximityDetection = false;
    bool m_gestureDetection = false;
};

#endif // PROXIMITY_GESTURE_CONTROL_H
