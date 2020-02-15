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

#pragma once

#include <QDataStream>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>

#include "logger.h"
#include "notifications.h"

class SoftwareUpdate : public QObject {
    Q_OBJECT

 public:
    explicit SoftwareUpdate(bool autoUpdate = true, QObject* parent = nullptr);
    ~SoftwareUpdate();

    Q_PROPERTY(qint64 bytesReceived READ bytesReceived NOTIFY bytesReceivedChanged)
    Q_PROPERTY(qint64 bytesTotal READ bytesTotal NOTIFY bytesTotalChanged)
    Q_PROPERTY(QString downloadSpeed READ downloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)

    Q_INVOKABLE void checkForUpdate();
    Q_INVOKABLE void downloadUpdate();
    Q_INVOKABLE void startUpdate();
    Q_INVOKABLE void startDockUpdate();

    void downloadUpdate(QUrl url);

    qint64  bytesReceived() { return m_bytesReceived; }
    qint64  bytesTotal() { return m_bytesTotal; }
    QString downloadSpeed() { return m_downloadSpeed; }

    bool autoUpdate() { return m_autoUpdate; }
    void setAutoUpdate(bool update);

    static SoftwareUpdate* getInstance() { return s_instance; }
    static QObject*        getQMLInstance(QQmlEngine* engine, QJSEngine* scriptEngine);

 signals:
    void updateAvailable(bool available);
    void bytesReceivedChanged();
    void bytesTotalChanged();
    void downloadSpeedChanged();
    void autoUpdateChanged();

 private:
    static SoftwareUpdate* s_instance;

    bool m_autoUpdate = true;

    QTimer* m_checkForUpdateTimer;

    qint64  m_bytesReceived = 0;
    qint64  m_bytesTotal    = 0;
    QString m_downloadSpeed;

    QUrl                   m_downloadUrl;
    QNetworkAccessManager* m_manager;
    QNetworkReply*         m_download = nullptr;
    QFile*                 m_file;
    QString                m_destination = "/usr/bin/yio-remote/downloads/latest.zip";
    QElapsedTimer*         m_downloadTimer;

 private slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void checkForUpdateFinished(QNetworkReply* reply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void onCheckForUpdateTimerTimeout();
};
