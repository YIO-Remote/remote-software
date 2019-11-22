#ifndef BLINDINTERFACE_H
#define BLINDINTERFACE_H

#include "entityinterface.h"

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class BlindDef : QObject
{
    Q_OBJECT
public:
    enum class Attributes { STATE, POSITION };
    Q_ENUM (Attributes)

    explicit BlindDef(QObject *parent = nullptr) : QObject(parent)
    {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class BlindInterface
{
public:
    virtual ~BlindInterface  ();

    virtual bool    state()         = 0;
    virtual int     position()      = 0;
};

QT_BEGIN_NAMESPACE
#define BlindInterface_iid "YIO.BlindInterface"
Q_DECLARE_INTERFACE(BlindInterface, BlindInterface_iid)
QT_END_NAMESPACE


#endif // BLINDINTERFACE_H
