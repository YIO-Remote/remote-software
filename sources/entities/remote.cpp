#include <QtDebug>
#include <QJsonArray>
#include "remote.h"
#include "../yioapi.h"

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
    m_commands = config.value("commands").toJsonArray().toVariantList();
    emit commandsChanged();
}
