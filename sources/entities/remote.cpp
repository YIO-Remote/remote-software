#include <QtDebug>
#include <QJsonArray>
#include "remote.h"
#include "../yioapi.h"

QString Remote::Type = "remote";

bool Remote::update(const QVariantMap &attributes)
{
}

void Remote::powerOn()
{
    command("POWER_ON", "");
}

void Remote::powerOff()
{
    command("POWER_OFF", "");
}

void Remote::powerToggle()
{
    command("POWER_TOGGLE", "");
}

void Remote::channelUp()
{
    command("CHANNEL_UP", "");
}

void Remote::channelDown()
{
    command("CHANNEL_DOWN", "");
}

void Remote::volumeUp()
{
    command("VOLUME_UP", "");
}

void Remote::volumeDown()
{
    command("VOLUME_DOWN", "");
}

void Remote::muteToggle()
{
    command("MUTE_TOGGLE", "");
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
