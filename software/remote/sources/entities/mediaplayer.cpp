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
        if (m_state == PLAYING || m_state == IDLE) {
            emit playing();
        }
    }
    if (m_volume != attributes.value("volume").toInt()) {
        m_volume = attributes.value("volume").toInt();
        chg = true;
        emit volumeChanged();
    }
    if (m_mediaImage != attributes.value("mediaImage").toString()) {
        m_mediaImage = attributes.value("mediaImage").toString();
        chg = true;
        emit mediaImageChanged();
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
