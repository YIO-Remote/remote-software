#include <QtDebug>
#include <QJsonArray>
#include "remote.h"
#include "../yioapi.h"

QString Remote::Type = "remote";

bool Remote::update(const QVariantMap &attributes)
{
    return false;
}

// transport and media controls
void Remote::play()
{
    command("PLAY", "");
}

void Remote::pause()
{
    command("PAUSE", "");
}

void Remote::playToggle()
{
    qDebug() << "POWERTOGGLE";
    command("PLAYTOGGLE", "");
}

void Remote::stop()
{
    command("STOP", "");
}

void Remote::forward()
{
    command("FORWARD", "");
}

void Remote::backward()
{
    command("BACKWARD", "");
}

void Remote::next()
{
    command("NEXT", "");
}

void Remote::previous()
{
    command("PREVIOUS", "");
}

void Remote::info()
{
    command("INFO", "");
}

void Remote::recordings()
{
    command("RECORDINGS", "");
}

void Remote::record()
{
    command("RECORD", "");
}

void Remote::live()
{
    command("LIVE", "");
}

// navigation
void Remote::cursorUp()
{
    command("CURSOR_UP", "");
}

void Remote::cursorDown()
{
    command("CURSOR_DOWN", "");
}

void Remote::cursorLeft()
{
    command("CURSOR_LEFT", "");
}

void Remote::cursorRight()
{
    command("CURSOR_RIGHT", "");
}

void Remote::cursorOK()
{
    command("CURSOR_OK", "");
}

void Remote::back()
{
    command("BACK", "");
}

void Remote::home()
{
    command("HOME", "");
}

void Remote::menu()
{
    command("MENU", "");
}

void Remote::exit()
{
    command("EXIT", "");
}

void Remote::app()
{
    command("APP", "");
}

// power commands
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

// tuner commands
void Remote::channelUp()
{
    command("CHANNEL_UP", "");
}

void Remote::channelDown()
{
    command("CHANNEL_DOWN", "");
}

void Remote::channelSearch()
{
    command("CHANNEL_SEARCH", "");
}

void Remote::favorite()
{
    command("FAVORITE", "");
}

void Remote::guide()
{
    command("GUIDE", "");
}

// volume commands
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
