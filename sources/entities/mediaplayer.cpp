#include <QtDebug>

#include "mediaplayer.h"
#include "entities.h"

MediaPlayerInterface::~MediaPlayerInterface()
{}
QMetaEnum   MediaPlayer::s_metaEnum;
QString     MediaPlayer::Type = "media_player";

bool MediaPlayer::update(const QVariantMap &attributes)
{
    bool chg = false;
    for (QVariantMap::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
        if (updateAttrByName (iter.key(), iter.value()))
            chg = true;
    }
    return chg;
}
bool MediaPlayer::updateAttrByName (const QString& name, const QVariant& value)
{
    int attrIndex = getAttrIndex(name);
    return updateAttrByIndex (attrIndex, value);
}
bool MediaPlayer::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool    chg = false;
    States  state;
    switch (static_cast<MediaPlayerDef::Attributes>(attrIndex)) {
        case MediaPlayerDef::Attributes::STATE:
            state = static_cast<States>(value.toInt());
            if (m_state != state) {
                m_state = state;
                if (m_state == States::PLAYING)
                    Entities::getInstance()->addMediaplayersPlaying(entity_id());
                else if (m_state == States::IDLE || m_state == States::OFF)
                    Entities::getInstance()->removeMediaplayersPlaying(entity_id());
                chg = true;
                emit stateChanged();
            }
            break;
        case MediaPlayerDef::Attributes::SOURCE:
            if (m_source != value.toString()) {
                m_source = value.toString();
                chg = true;
                emit sourceChanged();
            }
            break;
        case MediaPlayerDef::Attributes::VOLUME:
            if (m_volume != value.toInt()) {
                m_volume = value.toInt();
                chg = true;
                emit volumeChanged();
            }
            break;
        case MediaPlayerDef::Attributes::MUTED:
            if (m_muted != value.toBool()) {
                m_muted = value.toBool();
                chg = true;
                emit mutedChanged();
            }
            break;
        case MediaPlayerDef::Attributes::MEDIATYPE:
            if (m_mediaType != value.toString()) {
                m_mediaType = value.toString();
                chg = true;
                emit mediaTypeChanged();
            }
            break;
        case MediaPlayerDef::Attributes::MEDIATITLE:
            if (m_mediaTitle != value.toString()) {
                m_mediaTitle = value.toString();
                chg = true;
                emit mediaTitleChanged();
            }
            break;
        case MediaPlayerDef::Attributes::MEDIAARTIST:
            if (m_mediaArtist != value.toString()) {
                m_mediaArtist = value.toString();
                chg = true;
                emit mediaArtistChanged();
            }
            break;
        case MediaPlayerDef::Attributes::MEDIAIMAGE:
            if (m_mediaImage != value.toString()) {
                m_mediaImage = value.toString();
                chg = true;
                emit mediaImageChanged();
            }
            break;
        case MediaPlayerDef::Attributes::BROWSERESULT:
            m_browseResult = value;
            chg = true;
            emit browseResultChanged();
            break;
        }
    return chg;
}
void* MediaPlayer::getSpecificInterface()
{
    return qobject_cast<MediaPlayerInterface*>(this);
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

void MediaPlayer::setVolume(double value)
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
void MediaPlayer::search(const QString& searchString, const QString& itemKey)
{
    command("search:" + searchString, itemKey);
}

MediaPlayer::MediaPlayer(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity(Type, config, integrationObj, parent)
{
    if (!s_metaEnum.isValid()) {
        int index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("Attributes");
        s_metaEnum = MediaPlayerDef::staticMetaObject.enumerator(index);
    }
}

QStringList MediaPlayer::allAttributes ()
{
    QStringList list;
    for (int i = 0; i < s_metaEnum.keyCount(); i++)
        list.append(s_metaEnum.key(i));
    return list;
}
QString MediaPlayer::getAttrName(int attrIndex)
{
    return s_metaEnum.valueToKey(static_cast<int>(attrIndex));
}
int MediaPlayer::getAttrIndex(const QString& str)
{
    return s_metaEnum.keyToValue(str.toUpper().toUtf8());
}


