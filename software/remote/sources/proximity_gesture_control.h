#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QtDebug>
#include <QThread>

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
            apds.setProximityGain(2);
            apds.setProximityIntLowThreshold(0);
            apds.setProximityIntHighThreshold(m_proximitySetting);
            apds.clearProximityInt();
            apds.setLEDBoost(0);
            if ( !apds.enableProximitySensor(true) ){
                qDebug() << "Cannot initialize the proximity sensor";
                //: Error message that shows up as notification when the proximity sensor cannot be initialized
                m_apds9960Error = tr("Cannot initialize the proximity sensor. Please restart the remote.");
                emit apds9960Notify();
            }

        } else {
            // turn off
            apds.disableProximitySensor();
        }
#endif
    }

    Q_INVOKABLE void gestureDetection(bool state)
    {
        m_gestureDetection = state;
#ifdef __arm__
        if (state) {
            // turn on
            apds.setGestureGain(0);
            apds.setLEDBoost(0);
            apds.enableGestureSensor(true);

        } else {
            // turn off
            apds.disableGestureSensor();
        }
#endif
    }

    Q_INVOKABLE void readInterrupt() {
#ifdef __arm__
        if (m_proximityDetection) {

            qDebug() << "Proximity Interrupt";

            // read the value
            apds.readProximity(m_proximity);
            delay(200);

            // clear the interrupt
            apds.clearProximityInt();

            // turn off proximity detection
            proximityDetection(false);

            qDebug() << "Proximity reading:" << m_proximity;

            if (m_proximity > m_proximitySetting) {
                qDebug() << "Proximity reading OK";

                apds.enableLightSensor(false);
                // read the ambient light
                apds.readAmbientLight(m_ambientLight);

                // let qml know
                emit proximityEvent();

                delay(100);

            } else {
                qDebug() << "Restarting proximity detection";
                delay(100);
                proximityDetection(true);
            }

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
#endif
    }

public:
    ProximityGestureControl()
    {
#ifdef __arm__
        if ( !apds.init() )
        {
            qDebug() << "Cannot initialize the proximity sensor";
            //: Error message that shows up as notification when the proximity sensor cannot be initialized
            m_apds9960Error = tr("Cannot initialize the proximity sensor. Please restart the remote.");
            emit apds9960Notify();
        }

        if ( !apds.enableLightSensor(false) ) {
            qDebug() << "Cannot initialize the light sensor";
            //: Error message that shows up as notification when the light sensor cannot be initialized
            m_apds9960Error = tr("Cannot initialize the light sensor. Please restart the remote.");
            emit apds9960Notify();
        }

        delay(500);

        // read the ambient light when it's on first
        if ( !apds.readAmbientLight(m_ambientLight)) {
            //: Error message that shows up as notification when light value cannot be read
            m_apds9960Error = tr("Error reading light values.");
            emit apds9960Notify();
        } else {
            apds.disableLightSensor();
            delay(200);
            //            gestureDetection(true);
        }
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
    int m_proximitySetting = 100;
};

#endif // PROXIMITY_GESTURE_CONTROL_H
