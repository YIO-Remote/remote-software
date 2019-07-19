#include <QtDebug>

#include "mediaplayer.h"

QString MediaPlayer::Type = "media_player";

bool MediaPlayer::update(const QVariantMap &attributes)
{
    bool chg = false;
    if (m_state != static_cast<states>(attributes.value("state").toInt())) {
        m_state = static_cast<states>(attributes.value("state").toInt());
        chg = true;
        emit stateChanged();
    }
    return  chg;
}

MediaPlayer::MediaPlayer() :
    Entity(Type, QVariantMap(), NULL)
{
}

MediaPlayer::MediaPlayer(const QVariantMap& config, QObject* integrationObj):
    Entity(Type, config, integrationObj)
{
    init(config["attributes"].toMap());
}


void MediaPlayer::init(const QVariantMap &config)
{
    m_state = static_cast<states>(config.value("state").toInt());
    emit stateChanged();
}
