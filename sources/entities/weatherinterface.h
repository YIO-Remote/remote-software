#ifndef WEATHERINTERFACE_H
#define WEATHERINTERFACE_H

#include <QObject>

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class WeatherDef : public QObject
{
    Q_OBJECT
public:
    enum Attributes {  STATE, CURRENT, FORECAST };
    Q_ENUM (Attributes)

    enum States     { OFFLINE, ONLINE };
    Q_ENUM (States)

    explicit WeatherDef(QObject *parent = nullptr) : QObject(parent)
    {}
};

// Specifc interface is not required

#endif // WEATHERINTERFACE_H
