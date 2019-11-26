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

    Q_PROPERTY  (bool           state       READ    state       NOTIFY stateChanged)
    Q_PROPERTY  (int            position    READ    position    NOTIFY positionChanged)

    // update an entity's attributes
    Q_INVOKABLE bool            update              (const QVariantMap& attributes) override;
    Q_INVOKABLE bool            updateAttrByName    (const QString& name, const QVariant& value) override;
    Q_INVOKABLE bool            updateAttrByIndex   (int attrIndex, const QVariant& value) override;

    // attribute name and index
    Q_INVOKABLE QString         getAttrName         (int attrIndex) override;
    Q_INVOKABLE int             getAttrIndex        (const QString& attrName) override;

    // blind commands
    Q_INVOKABLE void            close();
    Q_INVOKABLE void            open();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            setPosition(int value);

    // only for C++ integrations
    virtual     void*           getSpecificInterface() override;

    Blind(const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

    bool                        state() override    { return m_state; }
    int                         position() override { return m_position; }

signals:
    void stateChanged();
    void positionChanged();

public:
    static QString Type;

    static QStringList&         AllAttributes()
    {
        static QStringList s;
        if (s.count() == 0)
            s.append(allAttributes());
        return s;
    }

private:
    static QStringList          allAttributes();
    static QMetaEnum            s_metaEnum;

    bool                        m_state;
    int                         m_position;
};

#endif // BLIND_H
