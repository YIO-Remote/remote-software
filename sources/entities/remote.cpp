#include <QtDebug>
#include <QJsonArray>
#include "remote.h"
#include "../yioapi.h"

QString Remote::Type = "remote";

bool Remote::update(const QVariantMap &attributes)
{
}

Remote::Remote(QObject *parent) :
    Entity (Type, QVariantMap(), NULL, parent)
{

}

Remote::Remote(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    m_commands = config.value("commands").toJsonArray().toVariantList();
    emit commandsChanged();
}
