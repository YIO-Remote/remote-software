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
    //    Q_PROPERTY(QString gesture READ getgesture)

    int getambientLight() {
        return (int) m_ambientLight;
    }

    Q_INVOKABLE void proximityDetection(bool state)
    {
        if (state) {
            // turn on
            m_proximityDetection = state;

            apds.setProximityGain(1);
            apds.setProximityIntLowThreshold(0);
            apds.setProximityIntHighThreshold(110);
            apds.clearProximityInt();
            apds.enableProximitySensor(true);
            apds.setLEDBoost(0);

        } else {
            // turn off
            apds.disableProximitySensor();
        }
    }

    Q_INVOKABLE void gestureDetection(bool state)
    {
        if (state) {
            // turn on
            m_gestureDetection = state;

            apds.enableGestureSensor();
            apds.setGestureGain(0);
            apds.setLEDBoost(0);

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

            // enable gesture detection
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
            qDebug() << "Ambient light:" << int(m_ambientLight);

            apds.disableLightSensor();
            delay(500);
            proximityDetection(true);
        }
    }

    ~ProximityGestureControl()
    {

    }

signals:
    void proximityEvent();

private:
    APDS9960 apds = APDS9960();
    uint16_t m_ambientLight;
    uint8_t m_proximity;
    QString m_gesture;
    bool m_proximityDetection = false;
    bool m_gestureDetection = false;
};

#endif // PROXIMITY_GESTURE_CONTROL_H
