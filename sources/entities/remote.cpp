#include <QtDebug>
#include <QJsonArray>
#include "remote.h"
#include "../yioapi.h"

RemoteInterface::~RemoteInterface()
{
}

QString   Remote::Type = "remote";
QMetaEnum Remote::s_metaEnumCommands;
QMetaEnum Remote::s_metaEnumAttr;
QMetaEnum Remote::s_metaEnumFeatures;
QMetaEnum Remote::s_metaEnumState;

void Remote::staticInitialize()
{
    if (!s_metaEnumAttr.isValid()) {
        int index = RemoteDef::staticMetaObject.indexOfEnumerator("Attributes");
        s_metaEnumAttr = RemoteDef::staticMetaObject.enumerator(index);
        index = RemoteDef::staticMetaObject.indexOfEnumerator("States");
        s_metaEnumState = RemoteDef::staticMetaObject.enumerator(index);
        index = RemoteDef::staticMetaObject.indexOfEnumerator("Features");
        s_metaEnumFeatures = RemoteDef::staticMetaObject.enumerator(index);
        index = RemoteDef::staticMetaObject.indexOfEnumerator("Commands");
        s_metaEnumCommands = RemoteDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<RemoteDef>("Entity.Remote", 1, 0, "Remote", "Not creatable as it is an enum type.");
    }
}

// transport and media controls
void Remote::play()
{
    command(RemoteDef::C_PLAY, "");
}

void Remote::pause()
{
    command(RemoteDef::C_PAUSE, "");
}

void Remote::playToggle()
{
    qDebug() << "POWERTOGGLE";
    command(RemoteDef::C_PLAYTOGGLE, "");
}

void Remote::stop()
{
    command(RemoteDef::C_STOP, "");
}

void Remote::forward()
{
    command(RemoteDef::C_FORWARD, "");
}

void Remote::backward()
{
    command(RemoteDef::C_BACKWARD, "");
}

void Remote::next()
{
    command(RemoteDef::C_NEXT, "");
}

void Remote::previous()
{
    command(RemoteDef::C_PREVIOUS, "");
}

void Remote::info()
{
    command(RemoteDef::C_INFO, "");
}

void Remote::recordings()
{
    command(RemoteDef::C_RECORDINGS, "");
}

void Remote::record()
{
    command(RemoteDef::C_RECORD, "");
}

void Remote::live()
{
    command(RemoteDef::C_LIVE, "");
}

// navigation
void Remote::cursorUp()
{
    command(RemoteDef::C_CURSOR_UP, "");
}

void Remote::cursorDown()
{
    command(RemoteDef::C_CURSOR_DOWN, "");
}

void Remote::cursorLeft()
{
    command(RemoteDef::C_CURSOR_LEFT, "");
}

void Remote::cursorRight()
{
    command(RemoteDef::C_CURSOR_RIGHT, "");
}

void Remote::cursorOK()
{
    command(RemoteDef::C_CURSOR_OK, "");
}

void Remote::back()
{
    command(RemoteDef::C_BACK, "");
}

void Remote::home()
{
    command(RemoteDef::C_HOME, "");
}

void Remote::menu()
{
    command(RemoteDef::C_MENU, "");
}

void Remote::exit()
{
    command(RemoteDef::C_EXIT, "");
}

void Remote::app()
{
    command(RemoteDef::C_APP, "");
}

// power commands
void Remote::powerOn()
{
    command(RemoteDef::C_POWER_ON, "");
}

void Remote::powerOff()
{
    command(RemoteDef::C_POWER_OFF, "");
}

void Remote::powerToggle()
{
    command(RemoteDef::C_POWER_TOGGLE, "");
}

// tuner commands
void Remote::channelUp()
{
    command(RemoteDef::C_CHANNEL_UP, "");
}

void Remote::channelDown()
{
    command(RemoteDef::C_CHANNEL_DOWN, "");
}

void Remote::channelSearch()
{
    command(RemoteDef::C_CHANNEL_SEARCH, "");
}

void Remote::toFavorite()
{
    command(RemoteDef::C_FAVORITE, "");
}

void Remote::guide()
{
    command(RemoteDef::C_GUIDE, "");
}

// volume commands
void Remote::volumeUp()
{
    command(RemoteDef::C_VOLUME_UP, "");
}

void Remote::volumeDown()
{
    command(RemoteDef::C_VOLUME_DOWN, "");
}

void Remote::muteToggle()
{
    command(RemoteDef::C_MUTE_TOGGLE, "");
}

Remote::Remote(QObject *parent) :
    Entity (Type, QVariantMap(), nullptr, parent)
{

}

Remote::Remote(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    staticInitialize();

    m_enumAttr = &s_metaEnumAttr;
    m_enumFeatures = &s_metaEnumFeatures;
    m_enumCommands = &s_metaEnumCommands;
    m_enumState = &s_metaEnumState;

    m_specificInterface = qobject_cast<RemoteInterface*>(this);
    initializeSupportedFeatures(config);
    m_commands = config.value("commands").toJsonArray().toVariantList();
    emit commandsChanged();
}
