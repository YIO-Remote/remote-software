#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "entity.h"

class Remote : public Entity
{
    Q_OBJECT

public:

    Q_INVOKABLE bool            update(const QVariantMap& attributes) override;

    // remote commands


    explicit Remote();
    Remote(const QVariantMap& config, QObject* integrationObj);

signals:

public:
    static QString      Type;

private:

};

#endif // REMOTE_H
