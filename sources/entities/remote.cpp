#include <QtDebug>
#include "remote.h"

QString Remote::Type = "remote";

bool Remote::update(const QVariantMap &attributes)
{
}

Remote::Remote() :
    Entity (Type, QVariantMap(), NULL)
{

}

Remote::Remote(const QVariantMap& config, QObject* integrationObj):
    Entity (Type, config, integrationObj)
{
}
