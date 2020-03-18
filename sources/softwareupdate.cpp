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
#include <QRegExp>
#include <QUrlQuery>

#include "config.h"
#include "environment.h"
#include "notifications.h"
#include "standbycontrol.h"

static const QString UPDATE_BASENAME = "latest.";
static const QString META_FILENAME = "latest.version";

static Q_LOGGING_CATEGORY(CLASS_LC, "softwareupdate");

SoftwareUpdate *SoftwareUpdate::s_instance = nullptr;

SoftwareUpdate::SoftwareUpdate(const QVariantMap &cfg, BatteryFuelGauge *batteryFuelGauge, QObject *parent)
    : QObject(parent),
      m_batteryFuelGauge(batteryFuelGauge),
      m_autoUpdate(cfg.value("autoUpdate", false).toBool()),
      m_initialCheckDelay(cfg.value("checkDelay", 60).toInt()),
      m_baseUpdateUrl(cfg.value("updateUrl", "https://update.yio.app/v1").toUrl()),
      m_downloadDir(cfg.value("downloadDir", "/tmp/yio").toString()) {
    Q_ASSERT(m_batteryFuelGauge);

    s_instance = this;
    m_downloadDir.makeAbsolute();

    int checkIntervallSec = cfg.value("checkInterval", 3600).toInt();  // 1 hour
    if (checkIntervallSec < 600) {
        qCInfo(CLASS_LC) << "Adjusting check intervall to 600s because configured value is too short!";
        checkIntervallSec = 600;
    }

    qCDebug(CLASS_LC) << "Auto update:" << m_autoUpdate << ", base url:" << m_baseUpdateUrl.toString()
                      << ", download dir:" << m_downloadDir.path();

    m_checkForUpdateTimer.setInterval(checkIntervallSec * 1000);
    connect(&m_checkForUpdateTimer, &QTimer::timeout, this, &SoftwareUpdate::onCheckForUpdateTimerTimeout);

    connect(&m_manager, &QNetworkAccessManager::finished, this, &SoftwareUpdate::onCheckForUpdateFinished);

    connect(&m_fileDownload, &FileDownload::downloadProgress, this, &SoftwareUpdate::onDownloadProgress);
    connect(&m_fileDownload, &FileDownload::downloadComplete, this, &SoftwareUpdate::onDownloadComplete);
    connect(&m_fileDownload, &FileDownload::downloadFailed, this, &SoftwareUpdate::onDownloadFailed);
}

SoftwareUpdate::~SoftwareUpdate() {
    s_instance = nullptr;
    if (m_checkForUpdateTimer.isActive()) {
        m_checkForUpdateTimer.stop();
    }
}

void SoftwareUpdate::start() {
    if (m_autoUpdate) {
        // start update checker timer
        setAutoUpdate(true);

        // check for update as well after startup delay.
        // WiFi might not yet be ready and update check might delay initial screen loading!
        QTimer::singleShot(m_initialCheckDelay * 1000, this, &SoftwareUpdate::checkForUpdate);
    }
}

QObject *SoftwareUpdate::getQMLInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *instance = s_instance;
    engine->setObjectOwnership(instance, QQmlEngine::CppOwnership);
    return instance;
}

void SoftwareUpdate::setAutoUpdate(bool update) {
    m_autoUpdate = update;
    emit autoUpdateChanged();
    qCDebug(CLASS_LC) << "Autoupdate:" << m_autoUpdate;

    if (update == false) {
        qCDebug(CLASS_LC) << "Stopping software update timer";
        m_checkForUpdateTimer.stop();
    } else if (!m_checkForUpdateTimer.isActive()) {
        qCDebug(CLASS_LC) << "Starting software update timer. Interval:" << m_checkForUpdateTimer.interval() / 1000
                          << "s";
        m_checkForUpdateTimer.start();
    }
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

    Environment env;
    QUrlQuery query;
    query.addQueryItem("version", currentVersion());
    query.addQueryItem("device", env.getDeviceType());
    // TODO(zehnm) add OS version query parameter
    QUrl updateUrl = m_baseUpdateUrl.resolved(QUrl("updates/app"));
    updateUrl.setQuery(query);

    QNetworkRequest request(updateUrl);
    request.setRawHeader("Accept", "application/json");

    qCDebug(CLASS_LC) << "Checking for update:" << updateUrl.toString();
    m_manager.get(request);
    //  the get request will signal onCheckForUpdateFinished() with QNetworkReply where it will also be deleted
}

void SoftwareUpdate::onCheckForUpdateFinished(QNetworkReply *reply) {
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // handle well defined API status codes separately. Treat all others as connection error.
    if (!(status == 200 || status == 400 || status == 501 || status == 503)) {
        qCWarning(CLASS_LC) << "Network reply error: " << reply->error() << reply->errorString();
        Notifications::getInstance()->add(true, tr("Cannot connect to the update server."));
        reply->deleteLater();
        return;
    }

    QByteArray    responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject   jsonObject = jsonResponse.object();
    reply->deleteLater();

    if (status == 200) {
        m_downloadUrl.setUrl(jsonObject["url"].toString());
        m_newVersion = jsonObject["latest"].toString();

        // Make sure returned data is valid
        QRegExp rx("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}(-\\w+)?$");
        if (rx.indexIn(m_newVersion) != 0) {
            qCWarning(CLASS_LC) << "Invalid new version:" << m_newVersion;
            status = 500;
        }
        if (!m_downloadUrl.isValid()) {
            qCWarning(CLASS_LC) << "Invalid download URL:" << m_downloadUrl;
            status = 500;
        }
    }

    if (status != 200) {
        m_newVersion.clear();
        m_downloadUrl.clear();
        QString error;
        switch (status) {
            case 400:
                error = tr("Invalid request.");
                break;
            case 501:
                error = tr("Unsupported device.");
                break;
            case 503:
                error = tr("Service currently not available.");
                break;
            default:
                error = tr("Request error %1").arg(status);
        }

        qCWarning(CLASS_LC) << "Update check failed:" << status << jsonObject;
        Notifications::getInstance()->add(true, tr("Software update:") + " " + error);
        return;
    }

    m_updateAvailable = false;

    if (isAlreadyDownloaded(m_newVersion)) {
        qCInfo(CLASS_LC) << "Update has already been downloaded:" << m_newVersion;
        // trigger install screen
        m_updateAvailable = true;
        emit downloadComplete();
        emit installAvailable();
    } else {
        // we are only interested in newer version than the one we have
        if (isNewerVersion(currentVersion(), m_newVersion)) {
            m_updateAvailable = true;

            QObject *param = this;
            Notifications::getInstance()->add(
                false, tr("New software is available"), tr("Download"),
                [](QObject *param) { qobject_cast<SoftwareUpdate *>(param)->startDownload(); }, param);
        }
    }

    emit updateAvailableChanged();
}

bool SoftwareUpdate::startDownload() {
    if (!m_downloadUrl.isValid() || m_newVersion.isEmpty()) {
        qCWarning(CLASS_LC) << "Ignoring download: no valid download URL or version set!";
        m_updateAvailable = false;
        emit updateAvailableChanged();
        return false;
    }

    // start software update procedure if there's enough battery. Free space check is within FileDownload.
    if (!m_downloadDir.mkpath(m_downloadDir.path())) {
        qCCritical(CLASS_LC) << "Error creating download directory" << m_downloadDir.path();
        return false;
    }

    if (m_batteryFuelGauge->getLevel() < 50) {
        Notifications::getInstance()->add(true, tr("The remote needs to be at least 50% battery to perform updates."));
        return false;
    }

    QObject *obj = Config::getInstance()->getQMLObject("loader_second");
    obj->setProperty("source", "qrc:/basic_ui/settings/SoftwareupdateDownloading.qml");

    // TODO(zehnm) determine required size from update request?
    int requiredMB = 100;

    QString fileName = getDownloadFileName(m_downloadUrl);
    m_fileDownload.download(m_downloadUrl, m_downloadDir, fileName, requiredMB);

    return true;
}

void SoftwareUpdate::onDownloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal, const QString &speed) {
    Q_UNUSED(id)

    m_bytesReceived = bytesReceived;
    emit bytesReceivedChanged();

    m_bytesTotal = bytesTotal;
    emit bytesTotalChanged();

    m_downloadSpeed = speed;
    emit downloadSpeedChanged();
}

void SoftwareUpdate::onDownloadComplete(int id) {
    // create meta file containing version string
    QFile metafile(m_downloadDir.path() + "/" + META_FILENAME);
    if (!metafile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        onDownloadFailed(id, metafile.errorString());
        return;
    }

    QTextStream out(&metafile);
    out << m_newVersion;
    metafile.close();

    emit downloadComplete();
    emit installAvailable();
}

void SoftwareUpdate::onDownloadFailed(int id, QString errorMsg) {
    Q_UNUSED(id)
    qCWarning(CLASS_LC) << "Download of update failed:" << errorMsg;

    Notifications::getInstance()->add(true, tr("Update download failed: %1").arg(errorMsg));
    emit downloadFailed();
}

bool SoftwareUpdate::installAvailable() { return m_downloadDir.exists(META_FILENAME); }

bool SoftwareUpdate::performUpdate() {
    qCWarning(CLASS_LC) << "performUpdate() NOT YET IMPLEMENTED";
    Notifications::getInstance()->add(true, "Sorry, not yet implemented! Working on it :-)");
    return false;
}

bool SoftwareUpdate::startDockUpdate() {
    qCWarning(CLASS_LC) << "startDockUpdate() NOT YET IMPLEMENTED";
    return false;
}

bool SoftwareUpdate::isAlreadyDownloaded(const QString &version) {
    QFile metafile(m_downloadDir.path() + "/" + META_FILENAME);

    if (!metafile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&metafile);
    return version.compare(in.readLine(100), Qt::CaseInsensitive) == 0;
}

bool SoftwareUpdate::isNewerVersion(const QString &currentVersion, const QString &updateVersion) {
    // TODO(zehnm) support optional suffix, use regex
    QStringList newVersionDigits = updateVersion.split(".");
    QString     none = QString("%1").arg(newVersionDigits[0].toInt(), 2, 10, QChar('0'));
    QString     ntwo = QString("%1").arg(newVersionDigits[1].toInt(), 2, 10, QChar('0'));
    QString     nthree = QString("%1").arg(newVersionDigits[2].toInt(), 2, 10, QChar('0'));

    int combinedNewVersion;
    combinedNewVersion = (none.append(ntwo).append(nthree)).toInt();

    QStringList currentVersionDigits = currentVersion.split(".");
    QString     cone = QString("%1").arg(currentVersionDigits[0].toInt(), 2, 10, QChar('0'));
    QString     ctwo = QString("%1").arg(currentVersionDigits[1].toInt(), 2, 10, QChar('0'));
    QString     cthree = QString("%1").arg(currentVersionDigits[2].toInt(), 2, 10, QChar('0'));

    int combinedCurrentVersion;
    combinedCurrentVersion = (cone.append(ctwo).append(cthree)).toInt();

    // check if it's a newer version than we have
    if (combinedNewVersion > combinedCurrentVersion) {
        qCDebug(CLASS_LC) << "New version available:" << combinedNewVersion;
        return true;
    }
    qCDebug(CLASS_LC) << "Current version is up to date";
    return false;
}

QString SoftwareUpdate::getDownloadFileName(const QUrl &url) const {
    QString suffix = QFileInfo(url.path()).suffix().toLower();

    // TODO(zehnm) support other archives than .tar and .gz?
    if (suffix == "gz") {
        return UPDATE_BASENAME + suffix;
    }

    return UPDATE_BASENAME + "tar";
}
