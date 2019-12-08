#ifndef ENTITYINTERFACE_H
#define ENTITYINTERFACE_H

#include <QVariant>
#include "../integrations/integrationinterface.h"

/// This interface is used to access an entity from the integrations
class EntityInterface
{
public:
    virtual ~EntityInterface  ();

    // Access to the generic attributes
    virtual QString         type                () = 0;
    virtual QString         area                () = 0;
    virtual QString         friendly_name       () = 0;
    virtual QString         entity_id           () = 0;
    virtual QString         integration         () = 0;
    virtual QStringList     supported_features  () = 0;
    virtual bool            favorite            () = 0;
    virtual void            setFavorite         (bool value)  = 0;
    virtual IntegrationInterface* integrationObj() = 0;

    // State handling functions
    virtual int             state               () = 0;
    virtual bool            setState            (int value) = 0;
    virtual QString         stateText           () = 0;
    virtual bool            setStateText        (const QString& stateText) = 0;
    virtual bool            isOn                () = 0;
    virtual bool            supportsOn          () = 0;

    // connected state
    virtual bool            connected           () = 0;
    virtual void            setConnected        (bool value) = 0;

    // update an entity's attributes
    virtual bool            update              (const QVariantMap& attributes) = 0;
    virtual bool            updateAttrByName    (const QString& attrName, const QVariant& value) = 0;
    virtual bool            updateAttrByIndex   (int attrIndex, const QVariant& value) = 0;

    // check for feature
    virtual bool            isSupported         (const QString& feature) = 0;

    // attribute name and index
    virtual QString         getAttrName         (int attrIndex) = 0;
    virtual int             getAttrIndex        (const QString& attrName) = 0;

    // get entity specific interface, no better idea than void*, otherwise we need a base interface
    virtual void*           getSpecificInterface() = 0;     // light, blind, media player */

    virtual void            turnOn              () = 0;
    virtual void            turnOff             () = 0;
};

QT_BEGIN_NAMESPACE
#define EntityInterface_iid "YIO.EntityInterface"
Q_DECLARE_INTERFACE(EntityInterface, EntityInterface_iid)
QT_END_NAMESPACE

#endif // ENTITYINTERFACE_H
