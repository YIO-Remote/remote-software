#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QtDebug>

#include "apds9960.h"

//class ProximityGestureControl : public QObject
//{
//    Q_OBJECT

//public:
//    Q_PROPERTY(uint16_t ambientLight READ getambientLight)
//    Q_PROPERTY(Qstring gesture READ getgesture)

//    Q_INVOKABLE void proximityDetection(bool state)
//    {
//        if (state && !m_proximityDetection) {
//            // turn on
//            m_proximityDetection = state;

//            apds.enableProximitySensor();
//            apds.setProximityGain(3);
//            apds.setLEDBoost(0);
//        } else {
//            // turn off
//            apds.disableProximitySensor();
//        }
//    }

//    Q_INVOKABLE void GestureDetection(bool state)
//    {
//        if (state && !m_gestureDetection) {
//            // turn on
//            m_gestureDetection = state;

//            apds.enableGestureSensor();
//            apds.setGestureGain(0);
//            apds.setLEDBoost(0);

//        } else {
//            // turn off
//            apds.disableGestureSensor();
//        }
//    }

//public:
//    ProximityGestureControl()
//    {
//        if ( (false == apds.init()) || (false == apds.enableLightSensor(false)) )
//        {
//            qDebug() << "APDS-9960 init failed!";
//        }

//        delay(500);

//        // read the ambient light when it's on first

//        if ( !apds.readAmbientLight(m_ambientLight)) {
//            qDebug() << "Error reading light values" << endl;
//        } else {
//            qDebug() << "Ambient light: ";
//            qDebug() << int(m_ambientLight);
//        }
//    }

//    ~ProximityGestureControl()
//    {

//    }

//private:
//    APDS9960 apds = APDS9960();
//    uint16_t m_ambientLight;
//    QString m_gesture;
//    bool m_proximityDetection = true;
//    bool m_gestureDetection = false;
//};

#endif // PROXIMITY_GESTURE_CONTROL_H
