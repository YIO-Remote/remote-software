#ifndef BLIND_H
#define BLIND_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMetaEnum>
#include "entity.h"
#include "blindinterface.h"

class Blind : public Entity, BlindInterface
{
    Q_OBJECT
    Q_INTERFACES(BlindInterface)

public:
    Q_PROPERTY  (int                position    READ    position    NOTIFY positionChanged)

    // update an entity's attributes

    // blind commands
    Q_INVOKABLE void            close();
    Q_INVOKABLE void            open();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            setPosition(int value);

    bool                        updateAttrByIndex   (int attrIndex, const QVariant& value) override;

    void                        turnOn() override               { open (); }
    void                        turnOff() override              { close (); }
    int                         position() override             { return m_position; }
    bool                        isOn() override                 { return m_state == BlindDef::CLOSED; }

    Blind(const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

signals:
    void positionChanged();

public:
    static QString Type;
private:
    int                         m_position;
};

#endif // BLIND_H
