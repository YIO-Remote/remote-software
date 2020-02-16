/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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

#include "softwareupdate.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "softwareupdate");

SoftwareUpdate *SoftwareUpdate::s_instance = nullptr;

SoftwareUpdate::SoftwareUpdate(bool autoUpdate, QString appPath, BatteryFuelGauge *batteryFuelGauge, QObject *parent)
    : QObject(parent), m_batteryFuelGauge(batteryFuelGauge), m_autoUpdate(autoUpdate), m_appPath(appPath) {
    s_instance = this;

    m_downloadTimer = new QElapsedTimer();

    // start update checker timer
    m_checkForUpdateTimer = new QTimer(this);
    m_checkForUpdateTimer->setInterval(3600000);  // 1 hour
    connect(m_checkForUpdateTimer, &QTimer::timeout, this, &SoftwareUpdate::onCheckForUpdateTimerTimeout);
    m_checkForUpdateTimer->start();

    qCDebug(CLASS_LC) << "Current version:" << m_currentVersion;

    // check for update on startup as well
    checkForUpdate();
}

SoftwareUpdate::~SoftwareUpdate() {
    s_instance = nullptr;
    m_checkForUpdateTimer->stop();
    m_checkForUpdateTimer->deleteLater();
}

QObject *SoftwareUpdate::getQMLInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *instance = s_instance;
    engine->setObjectOwnership(instance, QQmlEngine::CppOwnership);
    return instance;
}

void SoftwareUpdate::onCheckForUpdateTimerTimeout() { checkForUpdate(); }

void SoftwareUpdate::checkForUpdate() {
    m_manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    connect(m_manager, &QNetworkAccessManager::finished, this, &SoftwareUpdate::checkForUpdateFinished);

    request.setUrl(QUrl(m_updateUrl));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");

    QJsonObject json;
    json.insert("version", m_currentVersion);
    QJsonDocument jsonDoc(json);
    QByteArray    jsonData = jsonDoc.toJson();

    m_manager->post(request, jsonData);
}

void SoftwareUpdate::checkForUpdateFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray    responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject   jsonObject   = jsonResponse.object();

        if (jsonObject.contains("error")) {
            qCWarning(CLASS_LC) << "Error" << jsonObject["error"].toString();
            Notifications::getInstance()->add(true, tr("Software update: ") + jsonObject["error"].toString());
        } else {
            m_downloadUrl = jsonObject["url"].toString();
            m_newVersion  = jsonObject["latest"].toString();
            m_newVersion.remove(0, 1);  // remove the "v"

            qCDebug(CLASS_LC) << "Url:" << m_downloadUrl << "Version:" << m_newVersion;

            QStringList newVersionDigits = m_newVersion.split(".");
            QString     none             = QString("%1").arg(newVersionDigits[0].toInt(), 2, 10, QChar('0'));
            QString     ntwo             = QString("%1").arg(newVersionDigits[1].toInt(), 2, 10, QChar('0'));
            QString     nthree           = QString("%1").arg(newVersionDigits[2].toInt(), 2, 10, QChar('0'));

            int combinedNewVersion;
            combinedNewVersion = (none.append(ntwo).append(nthree)).toInt();

            QStringList currentVersionDigits = m_currentVersion.split(".");
            QString     cone                 = QString("%1").arg(currentVersionDigits[0].toInt(), 2, 10, QChar('0'));
            QString     ctwo                 = QString("%1").arg(currentVersionDigits[1].toInt(), 2, 10, QChar('0'));
            QString     cthree               = QString("%1").arg(currentVersionDigits[2].toInt(), 2, 10, QChar('0'));

            int combinedCurrentVersion;
            combinedCurrentVersion = (cone.append(ctwo).append(cthree)).toInt();

            qCDebug(CLASS_LC) << "New version:" << combinedNewVersion << "Current version:" << combinedCurrentVersion;

            // check if it's a newer version than we have
            if (combinedNewVersion > combinedCurrentVersion) {
                m_updateAvailable = true;
                emit updateAvailableChanged();

                // send a notification
                QObject *param = this;
                Notifications::getInstance()->add(
                    false, tr("New software is available"), tr("Update"),
                    [](QObject *param) {
                        SoftwareUpdate *i = qobject_cast<SoftwareUpdate *>(param);
                        i->startUpdate();
                    },
                    param);
            } else {
                m_updateAvailable = false;
                emit updateAvailableChanged();
            }
        }
    } else {
        qCWarning(CLASS_LC) << "Network reply error: " << reply->errorString();
        Notifications::getInstance()->add(true, tr("Cannot connect to the update server."));
    }

    reply->deleteLater();
    qCDebug(CLASS_LC) << "Network reply deleted";

    m_manager->deleteLater();
    qCDebug(CLASS_LC) << "Networkaccessmanager deleted";
}

void SoftwareUpdate::downloadUpdate(QUrl url) {
    qCDebug(CLASS_LC) << "Downloading file" << url.toString();

    qCDebug(CLASS_LC) << "DIR" << QDir(m_appPath).mkpath("../downloads");

    QString path = m_appPath.append("/../downloads/latest.zip");

    // open the file
    m_file = new QFile(path);
    m_file->open(QIODevice::WriteOnly);
    qCDebug(CLASS_LC) << "File opened";

    // create network manager
    m_manager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    m_download = m_manager->get(request);

    connect(m_download, &QNetworkReply::downloadProgress, this, &SoftwareUpdate::downloadProgress);
    connect(m_download, &QNetworkReply::finished, this, &SoftwareUpdate::downloadFinished);

    qCDebug(CLASS_LC) << "Downloading" << url.toString();
    m_downloadTimer->start();
    qCDebug(CLASS_LC) << "Download timer started";
}

void SoftwareUpdate::setAutoUpdate(bool update) {
    m_autoUpdate = update;
    emit autoUpdateChanged();
    qCDebug(CLASS_LC) << "Autoupdate:" << m_autoUpdate;
}

void SoftwareUpdate::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    QByteArray  b = m_download->readAll();
    QDataStream out(m_file);
    out << b;

    m_bytesReceived = bytesReceived;
    emit bytesReceivedChanged();

    m_bytesTotal = bytesTotal;
    emit bytesTotalChanged();

    qCDebug(CLASS_LC) << "Bytes received:" << m_bytesReceived << "Bytes total:" << m_bytesTotal;

    double  speed = bytesReceived * 1000.0 / m_downloadTimer->elapsed();
    QString unit;
    if (speed < 1024) {
        unit = " bytes/sec";
    } else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = " kB/s";
    } else {
        speed /= 1024 * 1014;
        unit = " MB/s";
    }

    m_downloadSpeed = QString::number(speed);
    m_downloadSpeed.append(unit);
    emit downloadSpeedChanged();

    qCDebug(CLASS_LC) << "Download speed:" << m_downloadSpeed;
}

void SoftwareUpdate::downloadFinished() {
    m_file->close();
    qCDebug(CLASS_LC) << "File closed";

    m_file->deleteLater();
    qCDebug(CLASS_LC) << "File deleted";

    m_download->deleteLater();
    qCDebug(CLASS_LC) << "NetowrkReply deleted";

    m_manager->deleteLater();
    qCDebug(CLASS_LC) << "Networkaccessmanager deleted";

    emit downloadComplete();
}

void SoftwareUpdate::startUpdate() {
    // start software update procedure
    if (m_batteryFuelGauge->getLevel() < 50) {
        Notifications::getInstance()->add(true, tr("The remote needs to be at least 50% battery to perform updates."));
    } else {
        QObject *obj = Config::getInstance()->getQMLObject("loader_second");
        obj->setProperty("source", "qrc:/basic_ui/settings/SoftwareupdateDownloading.qml");

        downloadUpdate(m_downloadUrl);
    }
}

void SoftwareUpdate::startDockUpdate() {}
