#ifndef BLIND_H
#define BLIND_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "entity.h"

class Blind : public Entity
{
    Q_OBJECT

public:

    Q_PROPERTY  (bool           state       READ    state       NOTIFY stateChanged)
    Q_PROPERTY  (int            position    READ    position    NOTIFY positionChanged)

    Q_INVOKABLE bool            update(const QVariantMap& attributes) override;

    // blind commands
    Q_INVOKABLE void            close();
    Q_INVOKABLE void            open();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            setPosition(int value);

    explicit Blind();
    Blind(const QVariantMap& config, QObject* integrationObj);

    bool                        state() const { return m_state; }
    int                         position() const { return m_position; }

    void                        init(const QVariantMap& config);

signals:
    void stateChanged();
    void positionChanged();

public:
    static QString Type;

private:
    bool            m_state;
    int             m_position;
};

#endif // BLIND_H
