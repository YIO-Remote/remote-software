#include <QtDebug>
#include <QQmlApplicationEngine>

#include "mediaplayer.h"
#include "entities.h"

MediaPlayerInterface::~MediaPlayerInterface()
{}
QString MediaPlayer::Type = "media_player";

bool MediaPlayer::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool    chg = false;
    switch (attrIndex) {
        case MediaPlayerDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            if (chg) {
                if (m_state == MediaPlayerDef::PLAYING)
                    Entities::getInstance()->addMediaplayersPlaying(entity_id());
                else if (m_state == MediaPlayerDef::IDLE || m_state == MediaPlayerDef::OFF)
                    Entities::getInstance()->removeMediaplayersPlaying(entity_id());
            }
            break;
        case MediaPlayerDef::SOURCE:
            if (m_source != value.toString()) {
                m_source = value.toString();
                chg = true;
                emit sourceChanged();
            }
            break;
        case MediaPlayerDef::VOLUME:
            if (m_volume != value.toInt()) {
                m_volume = value.toInt();
                chg = true;
                emit volumeChanged();
            }
            break;
        case MediaPlayerDef::MUTED:
            if (m_muted != value.toBool()) {
                m_muted = value.toBool();
                chg = true;
                emit mutedChanged();
            }
            break;
        case MediaPlayerDef::MEDIATYPE:
            if (m_mediaType != value.toString()) {
                m_mediaType = value.toString();
                chg = true;
                emit mediaTypeChanged();
            }
            break;
        case MediaPlayerDef::MEDIATITLE:
            if (m_mediaTitle != value.toString()) {
                m_mediaTitle = value.toString();
                chg = true;
                emit mediaTitleChanged();
            }
            break;
        case MediaPlayerDef::MEDIAARTIST:
            if (m_mediaArtist != value.toString()) {
                m_mediaArtist = value.toString();
                chg = true;
                emit mediaArtistChanged();
            }
            break;
        case MediaPlayerDef::MEDIAIMAGE:
            if (m_mediaImage != value.toString()) {
                m_mediaImage = value.toString();
                chg = true;
                emit mediaImageChanged();
            }
            break;
        case MediaPlayerDef::BROWSERESULT:
            m_browseResult = value;
            chg = true;
            emit browseResultChanged();
            break;
        }
    return chg;
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

void MediaPlayer::pause()
{
    command("PAUSE", "");
}

void MediaPlayer::stop()
{
    command("STOP", "");
}

void MediaPlayer::previous()
{
    command("PREVIOUS", "");
}

void MediaPlayer::next()
{
    command("NEXT", "");
}

void MediaPlayer::setVolume(int value)
{
    command("VOLUME_SET", value);
}
void MediaPlayer::volumeUp()
{
    command("VOLUME_UP", "");
}
void MediaPlayer::volumeDown()
{
    command("VOLUME_DOWN", "");
}
// extension for "generic" media browsing
void MediaPlayer::browse(QString cmd)
{
    command("BROWSE", cmd);
}
void MediaPlayer::playMedia(const QString& cmd, const QString& itemKey)
{
    command("play:" + cmd, itemKey);
}
void MediaPlayer::search(const QString& searchString)
{
    command("SEARCH", searchString);
}

MediaPlayer::MediaPlayer(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent):
    Entity(Type, config, integrationObj, parent)
{
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = MediaPlayerDef::staticMetaObject.enumerator(index);
        index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = MediaPlayerDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<MediaPlayerDef>("Entity.MediaPlayer", 1, 0, "MediaPlayer", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<MediaPlayerInterface*>(this);
}

