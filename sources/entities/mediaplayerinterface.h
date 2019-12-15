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

#ifndef MEDIAPLAYERINTERFACE_H
#define MEDIAPLAYERINTERFACE_H

#include <QObject>
#include "../remote-software/components/media_player/sources/searchmodel_mediaplayer.h"

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class MediaPlayerDef : public QObject
{
    Q_OBJECT
public:
    enum Attributes  {  STATE, SOURCE, VOLUME, MUTED, MEDIATYPE, MEDIATITLE, MEDIAARTIST, MEDIAIMAGE, MEDIADURATION, MEDIAPROGRESS, BROWSERESULT, SEARCHRESULT };
    Q_ENUM (Attributes)

    enum States   { OFF=0, ON=1, IDLE=2, PLAYING=3 };
    Q_ENUM (States)

    explicit MediaPlayerDef (QObject *parent = nullptr) : QObject(parent)
    {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class MediaPlayerInterface
{
public:
    virtual ~MediaPlayerInterface  ();

    virtual int                         volume() = 0;
    virtual int                         mediaDuration() = 0;
    virtual int                         mediaProgress() = 0;
    virtual bool                        muted() = 0;
    virtual QString                     mediaType() = 0;
    virtual QString                     mediaImage() = 0;
    virtual QString                     mediaTitle() = 0;
    virtual QString                     mediaArtist() = 0;
    virtual QString                     source() = 0;

    // extension for "generic" media browsing
    virtual QVariant                    browseResult() = 0;
    virtual void                        setSearchModel(QObject* model) = 0;
    virtual void                        setBrowseModel(QObject* model) = 0;
};

QT_BEGIN_NAMESPACE
#define MediaPlayerInterface_iid "YIO.MediaPlayerInterface"
Q_DECLARE_INTERFACE(MediaPlayerInterface, MediaPlayerInterface_iid)
QT_END_NAMESPACE

#endif // MEDIAPLAYERINTERFACE_H
