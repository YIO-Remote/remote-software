/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include "mediaplayer.h"

#include <QQmlApplicationEngine>

#include "entities.h"

MediaPlayerInterface::~MediaPlayerInterface() {}
QString MediaPlayer::Type = "media_player";

bool MediaPlayer::updateAttrByIndex(int attrIndex, const QVariant &value) {
    bool chg = false;
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
        case MediaPlayerDef::MEDIADURATION:
            if (m_mediaDuration != value.toInt()) {
                m_mediaDuration = value.toInt();
                chg = true;
                emit mediaDurationChanged();
            }
            break;
        case MediaPlayerDef::MEDIAPROGRESS:
            if (m_mediaProgress != value.toInt()) {
                m_mediaProgress = value.toInt();
                chg = true;
                emit mediaProgressChanged();
            }
            break;
    }
    return chg;
}

void MediaPlayer::turnOn() {
    command(MediaPlayerDef::C_TURNON, "");
}

void MediaPlayer::turnOff() {
    command(MediaPlayerDef::C_TURNOFF, "");
}

void MediaPlayer::setrecentSearches(QVariant list) {
    m_recentSearches = list;
}

void MediaPlayer::play() {
    command(MediaPlayerDef::C_PLAY, "");
}

void MediaPlayer::pause() {
    command(MediaPlayerDef::C_PAUSE, "");
}

void MediaPlayer::stop() {
    command(MediaPlayerDef::C_STOP, "");
}

void MediaPlayer::previous() {
    command(MediaPlayerDef::C_PREVIOUS, "");
}

void MediaPlayer::next() {
    command(MediaPlayerDef::C_NEXT, "");
}

void MediaPlayer::setVolume(int value) {
    command(MediaPlayerDef::C_VOLUME_SET, value);
}
void MediaPlayer::volumeUp() {
    command(MediaPlayerDef::C_VOLUME_UP, "");
}
void MediaPlayer::volumeDown() {
    command(MediaPlayerDef::C_VOLUME_DOWN, "");
}

// navigation
void MediaPlayer::cursorUp() {
    command(MediaPlayerDef::C_UP, "");
}

void MediaPlayer::cursorDown() {
    command(MediaPlayerDef::C_DOWN, "");
}

void MediaPlayer::cursorLeft() {
    command(MediaPlayerDef::C_LEFT, "");
}

void MediaPlayer::cursorRight() {
    command(MediaPlayerDef::C_RIGHT, "");
}

void MediaPlayer::cursorOK() {
    command(MediaPlayerDef::C_OK, "");
}

void MediaPlayer::back() {
    command(RemoteDef::C_BACK, "");
}

void MediaPlayer::menu() {
    command(MediaPlayerDef::C_MENU, "");
}

// tuner commands
void MediaPlayer::channelUp() {
    command(MediaPlayerDef::C_CHANNEL_UP, "");
}

void MediaPlayer::channelDown() {
    command(MediaPlayerDef::C_CHANNEL_DOWN, "");
}

void MediaPlayer::muteToggle() {
    command(MediaPlayerDef::C_MUTE, "");
}

bool MediaPlayer::supportsOn() {
    if (isSupported(MediaPlayerDef::F_TURN_ON) && isSupported(MediaPlayerDef::F_TURN_OFF)) {
        return true;
    } else {
        return false;
    }
}
// extension for "generic" media browsing
void MediaPlayer::browse(QString cmd) {
    command(MediaPlayerDef::C_BROWSE, cmd);
}
void MediaPlayer::playMedia(const QString &itemKey, const QString &type) {
    QVariantMap map;
    map.insert("type", type);
    map.insert("id", itemKey);
    command(MediaPlayerDef::C_PLAY_ITEM, map);
}
void MediaPlayer::addToQueue(const QString &itemKey, const QString &type) {
    QVariantMap map;
    map.insert("type", type);
    map.insert("id", itemKey);
    command(MediaPlayerDef::C_QUEUE, map);
}
void MediaPlayer::search(const QString &searchText, const QString &itemKey) {
    QVariantMap map;
    map["id"] = itemKey;
    map["text"] = searchText;
    command(MediaPlayerDef::C_SEARCH_ITEM, map);
}
void MediaPlayer::search(const QString &searchString) {
    command(MediaPlayerDef::C_SEARCH, searchString);
}
void MediaPlayer::textInputMediaPlayer(const QString &textString) {
    command(MediaPlayerDef::C_MEDIAPLAYERTEXTINPUT, textString);
}
void MediaPlayer::getAlbum(const QString &id) {
    command(MediaPlayerDef::C_GETALBUM, id);
}
void MediaPlayer::getPlaylist(const QString &id) {
    command(MediaPlayerDef::C_GETPLAYLIST, id);
}
void MediaPlayer::getMediaPlayerChannelList(const QString &id) {
    command(MediaPlayerDef::C_GETMEDIAPLAYERCHANNELLIST, id);
}
void MediaPlayer::getMediaPlayerRemote(const QString &id) {
    command(MediaPlayerDef::C_GETMEDIAPLAYERREMOTE, id);
}
void MediaPlayer::getMediaPlayerEPGView(const QString &id) {
    command(MediaPlayerDef::C_GETMEDIAPLAYEREPGVIEW, id);
}

void MediaPlayer::setSearchModel(QObject *model) {
    m_searchModel = model;
    emit searchModelChanged();
}

void MediaPlayer::setBrowseModel(QObject *model) {
    m_browseModel = model;
    emit browseModelChanged(model);
}

MediaPlayer::MediaPlayer(const QVariantMap &config, IntegrationInterface *integrationObj, QObject *parent)
    : Entity(Type, config, integrationObj, parent),
      m_volume(0),
      m_muted(false),
      m_searchModel(nullptr),
      m_browseModel(nullptr) {
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = MediaPlayerDef::staticMetaObject.enumerator(index);
        index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = MediaPlayerDef::staticMetaObject.enumerator(index);
        index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = MediaPlayerDef::staticMetaObject.enumerator(index);
        index = MediaPlayerDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = MediaPlayerDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<MediaPlayerDef>("Entity.MediaPlayer", 1, 0, "MediaPlayer",
                                                   "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumFeatures = &metaEnumFeatures;
    m_enumCommands = &metaEnumCommands;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<MediaPlayerInterface *>(this);
    initializeSupportedFeatures(config);
}
