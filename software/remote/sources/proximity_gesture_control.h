#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QtDebug>

#include "apds9960.h"

class ProximityGestureControl : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(int ambientLight READ getambientLight)
    Q_PROPERTY(int proximity NOTIFY proximityEvent)
    Q_PROPERTY(QString gesture READ getgesture NOTIFY gestureEvent)

    int getambientLight() {
        return (int) m_ambientLight;
    }

    QString getgesture()
    {
        return m_gesture;
    }

    Q_INVOKABLE void proximityDetection(bool state)
    {
        m_proximityDetection = state;

        if (state) {
            // turn on
            apds.setProximityGain(1);
            apds.setProximityIntLowThreshold(0);
            apds.setProximityIntHighThreshold(110);
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
            apds.clearProximityInt();
            apds.enableLightSensor(false);
            delay(200);

            // read the ambient light
            apds.readAmbientLight(m_ambientLight);

            // let qml know
            emit proximityEvent();

            // turn off proximity detection
            proximityDetection(false);

            delay(1000);

            // enable gesture detection
            gestureDetection(true);

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
            qDebug() << "APDS-9960 init failed!";
        }

        delay(500);

        // read the ambient light when it's on first

        if ( !apds.readAmbientLight(m_ambientLight)) {
            qDebug() << "Error reading light values" << endl;
        } else {
            apds.disableLightSensor();
            delay(500);
            gestureDetection(true);
        }
    }

    ~ProximityGestureControl()
    {

    }

signals:
    void proximityEvent();
    void gestureEvent();

private:
    APDS9960 apds = APDS9960();
    uint16_t m_ambientLight;
    uint8_t m_proximity;
    QString m_gesture;
    bool m_proximityDetection = false;
    bool m_gestureDetection = false;
};

#endif // PROXIMITY_GESTURE_CONTROL_H
