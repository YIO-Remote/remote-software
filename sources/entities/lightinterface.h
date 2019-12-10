#ifndef LIGHTINTERFACE_H
#define LIGHTINTERFACE_H

#include <QObject>

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class LightDef : public QObject
{
    Q_OBJECT
public:
    enum Attributes     { STATE, BRIGHTNESS, COLOR, COLORTEMP };
    Q_ENUM (Attributes)

    enum States         { OFF=0, ON=1 };
    Q_ENUM (States)

    enum Features       { F_BRIGHTNESS, F_COLOR, F_COLORTEMP };
    Q_ENUM (Features)

    enum Commands       { C_OFF, C_ON, C_TOGGLE, C_BRIGHTNESS, C_COLOR, C_COLORTEMP };
    Q_ENUM (Commands)

    explicit LightDef(QObject *parent = nullptr) : QObject(parent)
    {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class LightInterface
{
public:
    virtual ~LightInterface  ();

    virtual int                 brightness()    = 0;
    virtual QColor              color()         = 0;
    virtual int                 colorTemp()     = 0;
};

QT_BEGIN_NAMESPACE
#define LightInterface_iid "YIO.LightInterface"
Q_DECLARE_INTERFACE(LightInterface, LightInterface_iid)
QT_END_NAMESPACE

#endif // LIGHTINTERFACE_H
