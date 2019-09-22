#include <QtDebug>

#include "mediaplayer.h"

QString MediaPlayer::Type = "media_player";

bool MediaPlayer::update(const QVariantMap &attributes)
{
    bool chg = false;
    if (attributes.contains("state") && m_state != static_cast<states>(attributes.value("state").toInt())) {
        m_state = static_cast<states>(attributes.value("state").toInt());
        if (m_state == PLAYING) {
            emit playing(objectName());
        } else {
            emit stopped(objectName());
        }
        chg = true;
        emit stateChanged();
    }

    if (attributes.contains("source") && m_source != attributes.value("source").toString()) {
        m_source = attributes.value("source").toString();
        chg = true;
        emit sourceChanged();
    }

    if (attributes.contains("volume") && m_volume != attributes.value("volume").toDouble()) {
        m_volume = attributes.value("volume").toDouble();
        chg = true;
        emit volumeChanged();
    }
    if (attributes.contains("mediaType") && m_mediaType != attributes.value("mediaType").toString()){
        m_mediaType = attributes.value("mediaType").toString();
        chg = true;
        emit mediaTypeChanged();
    }
    if (attributes.contains("mediaTitle") && m_mediaTitle != attributes.value("mediaTitle").toString()){
        m_mediaTitle = attributes.value("mediaTitle").toString();
        chg = true;
        emit mediaTitleChanged();
    }
    if (attributes.contains("mediaArtist") && m_mediaArtist != attributes.value("mediaArtist").toString()){
        m_mediaArtist = attributes.value("mediaArtist").toString();
        chg = true;
        emit mediaArtistChanged();
    }
    if (attributes.contains("mediaImage") && m_mediaImage != attributes.value("mediaImage").toString()) {
        m_mediaImage = attributes.value("mediaImage").toString();
        chg = true;
        emit mediaImageChanged();
    }
    return  chg;
}

void MediaPlayer::turnOn()
{
    command("TURNON", "");
}

void MediaPlayer::turnOff()
{
    command("TURNOFF", "");
}

void MediaPlayer::play()
{
    command("PLAY", "");
}

void MediaPlayer::previous()
{
    command("PREVIOUS", "");
}

void MediaPlayer::next()
{
    command("NEXT", "");
}

void MediaPlayer::setVolume(double value)
{
    command("VOLUME_SET", value);
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
