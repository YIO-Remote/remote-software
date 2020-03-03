/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#include <QDataStream>
#include <QDebug>
#include <QJsonDocument>
#include <QLoggingCategory>

#include "config.h"
#include "notifications.h"
#include "standbycontrol.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "softwareupdate");

SoftwareUpdate *SoftwareUpdate::s_instance = nullptr;

SoftwareUpdate::SoftwareUpdate(const QVariantMap &cfg, QString appPath, BatteryFuelGauge *batteryFuelGauge,
                               QObject *parent)
    : QObject(parent),
      m_batteryFuelGauge(batteryFuelGauge),
      m_checkIntervallSec(cfg.value("checkInterval", 3600).toInt()),  // 1 hour
      m_autoUpdate(cfg.value("autoUpdate").toBool()),
      m_updateUrl(cfg.value("updateUrl", "https://update.yio.app/update").toUrl()),
      m_downloadDir(cfg.value("downloadDir", "/tmp/yio").toString()),
      m_appPath(appPath),
      m_fileName("latest.gz") {
    s_instance = this;
    m_downloadDir.makeAbsolute();
    qCDebug(CLASS_LC) << "Auto update:" << m_autoUpdate << ", url:" << m_updateUrl.toString()
                      << ", download dir:" << m_downloadDir.path();

    connect(&m_fileDownload, &FileDownload::downloadProgress, this, &SoftwareUpdate::downloadProgress);
    connect(&m_fileDownload, &FileDownload::downloadSpeed, this, &SoftwareUpdate::onDownloadSpeed);
    connect(&m_fileDownload, &FileDownload::downloadComplete, this, &SoftwareUpdate::downloadFinished);
    connect(&m_fileDownload, &FileDownload::downloadFailed, this, &SoftwareUpdate::onDownloadFailed);
}

SoftwareUpdate::~SoftwareUpdate() {
    s_instance = nullptr;
    m_checkForUpdateTimer->stop();
    m_checkForUpdateTimer->deleteLater();
}

void SoftwareUpdate::start() {
    qCDebug(CLASS_LC) << "Starting software update timer. Interval:" << m_checkIntervallSec << "s";

    // start update checker timer
    m_checkForUpdateTimer = new QTimer(this);
    m_checkForUpdateTimer->setInterval(m_checkIntervallSec * 1000);
    connect(m_checkForUpdateTimer, &QTimer::timeout, this, &SoftwareUpdate::onCheckForUpdateTimerTimeout);
    m_checkForUpdateTimer->start();

    // check for update on startup as well
    // TODO(zehnm) delay initial check. WiFi might not yet be ready and update check might delay initial screen loading!
    checkForUpdate();

    // JUST TESTING !!!
    m_updateAvailable = true;
    emit updateAvailableChanged();
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
    // TODO(zehnm) enhance StandbyControl with isWifiAvailable() to encapsulate standby logic.
    //             This allows enhanced standby logic in the future (loose coupling).
    // only check update if standby mode is not WIFI_OFF
    if (StandbyControl::getInstance()->mode() == StandbyControl::WIFI_OFF) {
        qCDebug(CLASS_LC) << "Update check skipped: WiFi not available";
        return;
    }

    qCDebug(CLASS_LC) << "Checking for update. Current version:" << m_currentVersion;

    m_manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    connect(m_manager, &QNetworkAccessManager::finished, this, &SoftwareUpdate::checkForUpdateFinished);

    request.setUrl(m_updateUrl);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");

    // TODO(zehm) use GET with query parameters
    QJsonObject json;
    json.insert("version", m_currentVersion);
    json.insert("type", getDeviceType());

    QJsonDocument jsonDoc(json);
    QByteArray    jsonData = jsonDoc.toJson();

    m_manager->post(request, jsonData);
}

void SoftwareUpdate::checkForUpdateFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray    responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject   jsonObject = jsonResponse.object();

        if (jsonObject.contains("error")) {
            qCWarning(CLASS_LC) << "Error" << jsonObject["error"].toString();
            Notifications::getInstance()->add(true, tr("Software update:") + " " + jsonObject["error"].toString());
        } else {
            m_downloadUrl.setUrl(jsonObject["url"].toString());
            m_newVersion = jsonObject["latest"].toString();

            // always validate returned data! Never trust json!

            // TODO(zehnm) validate new version string

            if (!m_downloadUrl.isValid()) {
                qCWarning(CLASS_LC) << "Invalid download URL:" << m_downloadUrl;
                // TODO(zehnm) abort / error handling
            }

            // TODO(zehnm) Quick and dirty for now... Either use file ending from download link or agree an tar / gz
            m_fileName = "latest.tar";

            qCDebug(CLASS_LC) << "Url:" << m_downloadUrl << "Version:" << m_newVersion;

            QStringList newVersionDigits = m_newVersion.split(".");
            QString     none = QString("%1").arg(newVersionDigits[0].toInt(), 2, 10, QChar('0'));
            QString     ntwo = QString("%1").arg(newVersionDigits[1].toInt(), 2, 10, QChar('0'));
            QString     nthree = QString("%1").arg(newVersionDigits[2].toInt(), 2, 10, QChar('0'));

            int combinedNewVersion;
            combinedNewVersion = (none.append(ntwo).append(nthree)).toInt();

            QStringList currentVersionDigits = m_currentVersion.split(".");
            QString     cone = QString("%1").arg(currentVersionDigits[0].toInt(), 2, 10, QChar('0'));
            QString     ctwo = QString("%1").arg(currentVersionDigits[1].toInt(), 2, 10, QChar('0'));
            QString     cthree = QString("%1").arg(currentVersionDigits[2].toInt(), 2, 10, QChar('0'));

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
                    false, tr("New software is available"), tr("Download"),
                    [](QObject *param) { qobject_cast<SoftwareUpdate *>(param)->startDownload(); }, param);
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

void SoftwareUpdate::setAutoUpdate(bool update) {
    m_autoUpdate = update;
    emit autoUpdateChanged();
    qCDebug(CLASS_LC) << "Autoupdate:" << m_autoUpdate;
}

void SoftwareUpdate::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    m_bytesReceived = bytesReceived;
    emit bytesReceivedChanged();

    m_bytesTotal = bytesTotal;
    emit bytesTotalChanged();
}

void SoftwareUpdate::downloadFinished() {
    if (!m_fileDownload.hasError()) {
        // TODO(zehnm) validate download
        emit downloadComplete();
        emit installAvailable();
    } else {
        // TODO(zehnm) signal error instead of downloadComplete()?
        emit downloadComplete();
    }
}

void SoftwareUpdate::onDownloadFailed() {
    qCWarning(CLASS_LC) << "Download of update failed";
    // TODO(zehnm) signal error instead of downloadComplete()?
    emit downloadComplete();
}

void SoftwareUpdate::onDownloadSpeed(const QString &speed) {
    m_downloadSpeed = speed;
    emit downloadSpeedChanged();
}

bool SoftwareUpdate::startDownload() {
    // TODO(zehnm) verify that a download is not already in progress!

    // start software update procedure if there's enough free space and battery
    if (!m_downloadDir.mkpath(m_downloadDir.path())) {
        qCCritical(CLASS_LC) << "Error creating download directory" << m_downloadDir.path();
        return false;
    }

    // TODO(zehnm) determine required size from update request
    int requiredMB = 100;
    if (!m_fileDownload.checkDiskSpace(m_downloadDir, requiredMB)) {
        Notifications::getInstance()->add(
            true, tr("The remote requires %1 MB free space to download updates.").arg(requiredMB));
        return false;
    }
    if (m_batteryFuelGauge->getLevel() < 50) {
        Notifications::getInstance()->add(true, tr("The remote needs to be at least 50% battery to perform updates."));
        return false;
    }

    QObject *obj = Config::getInstance()->getQMLObject("loader_second");
    obj->setProperty("source", "qrc:/basic_ui/settings/SoftwareupdateDownloading.qml");

    return m_fileDownload.download(m_downloadUrl, m_downloadDir, "latest.zip");
}

bool SoftwareUpdate::installAvailable() { return m_downloadDir.exists(m_fileName); }

bool SoftwareUpdate::performUpdate() {
    qCWarning(CLASS_LC) << "performUpdate() NOT YET IMPLEMENTED";
    return false;
}

bool SoftwareUpdate::startDockUpdate() {
    qCWarning(CLASS_LC) << "startDockUpdate() NOT YET IMPLEMENTED";
    return false;
}

QString SoftwareUpdate::getDeviceType() {
    // Quick and dirty for now
    QString cpu =
#if defined(Q_PROCESSOR_X86_64)
        ":x86_64";
#elif defined(Q_PROCESSOR_X86_32)
        ":x86_32";
#elif defined(Q_PROCESSOR_ARM)
        ":arm";
#else
        "";
#endif

#if defined(Q_OS_ANDROID)
    return QString("android") + cpu;
#elif defined(Q_OS_IOS)
    return "ios";
#elif defined(Q_OS_LINUX)
#if defined(Q_PROCESSOR_ARM)
    // TODO(zehnm) further check if running on yio remote hardware or RPi!
    return "remote";
#else
    return QString("linux") + cpu;
#endif
#elif defined(Q_OS_WIN64)
    return "windows:64";
#elif defined(Q_OS_WIN32)
        return "windows:32";
#elif defined(Q_OS_MACOS)
        return "mac";
#else
        return "other";
#endif
}
