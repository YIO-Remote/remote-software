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
#include <QProcess>
#include <QUrlQuery>

#include "config.h"
#include "environment.h"
#include "notifications.h"
#include "standbycontrol.h"

static const QString UPDATE_BASENAME = "latest";
static const QString UPDATE_FILEMARKER = "latest.version";

static Q_LOGGING_CATEGORY(CLASS_LC, "softwareupdate");

SoftwareUpdate *SoftwareUpdate::s_instance = nullptr;

SoftwareUpdate::SoftwareUpdate(const QVariantMap &cfg, BatteryFuelGauge *batteryFuelGauge, QObject *parent)
    : QObject(parent),
      m_batteryFuelGauge(batteryFuelGauge),
      m_autoUpdate(cfg.value("autoUpdate", false).toBool()),
      m_initialCheckDelay(cfg.value("checkDelay", 60).toInt()),
      m_appUpdateUrl(cfg.value("updateUrl", "https://update.yio.app/v1/")
                         .toUrl()
                         .resolved(cfg.value("updateUrlAppPath", "app/updates").toUrl())),
      m_downloadDir(cfg.value("downloadDir", "/var/yio/download").toString()),
      m_appUpdateScript(cfg.value("appUpdateScript", "/opt/yio/scripts/app-update.sh").toString()),
      m_channel(cfg.value("channel", "release").toString()) {
    Q_ASSERT(m_batteryFuelGauge);

    s_instance = this;
    m_downloadDir.makeAbsolute();

    int checkIntervallSec = cfg.value("checkInterval", 3600).toInt();  // 1 hour
    if (checkIntervallSec < 600) {
        qCInfo(CLASS_LC) << "Adjusting check intervall to 600s because configured value is too short!";
        checkIntervallSec = 600;
    }

    qCDebug(CLASS_LC) << "Auto update:" << m_autoUpdate << ", app update url:" << m_appUpdateUrl.toString()
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

void SoftwareUpdate::setChannel(const QString &channel) {
    m_channel = channel;
    emit channelChanged();
    qCDebug(CLASS_LC) << "Software update channel:" << channel;
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
    QUrlQuery   query;
    query.addQueryItem("appVersion", currentVersion());
    if (env.getRemoteOsVersion() != Environment::UNKNOWN) {
        query.addQueryItem("osVersion", env.getRemoteOsVersion());
    }
    query.addQueryItem("device", QUrl::toPercentEncoding(env.getDeviceType()));
    query.addQueryItem("channel", m_channel);

    QUrl updateUrl = m_appUpdateUrl;
    updateUrl.setQuery(query);

    QNetworkRequest request(updateUrl);
    request.setRawHeader("Accept", "application/json");

    qCDebug(CLASS_LC) << "Checking for update:" << updateUrl;
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
        if (!jsonObject["available"].toBool(false)) {
            qCInfo(CLASS_LC) << "Current version is up to date";
            return;
        }
        m_downloadUrl.setUrl(jsonObject["url"].toString());
        m_newVersion = jsonObject["version"].toString();

        // Make sure returned data is valid
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
        m_updateAvailable = true;

        QObject *param = this;
        Notifications::getInstance()->add(
            false, tr("New software is available"), tr("Download"),
            [](QObject *param) { qobject_cast<SoftwareUpdate *>(param)->startDownload(); }, param);
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
    if (m_batteryFuelGauge->getLevel() < 50) {
        Notifications::getInstance()->add(true, tr("The remote needs to be at least 50% battery to perform updates."));
        return false;
    }

    if (!m_downloadDir.mkpath(m_downloadDir.path())) {
        qCCritical(CLASS_LC) << "Error creating download directory" << m_downloadDir.path();
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

void SoftwareUpdate::onDownloadComplete(int id, const QString &filePath) {
    // create meta file containing version string
    QFile metafile(m_downloadDir.path() + "/" + UPDATE_FILEMARKER);
    if (!metafile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qCCritical(CLASS_LC) << "Error creating update filemarker for downloaded file:" << filePath
                             << "Error:" << metafile.errorString();
        onDownloadFailed(id, metafile.errorString());
        return;
    }

    QTextStream out(&metafile);
    out << "Update\t:\t" << filePath << "\n";
    out << "Version\t:\t" << m_newVersion << "\n";
    out << "Url\t:\t" << m_downloadUrl.toString() << "\n";
    metafile.close();

    qCInfo(CLASS_LC) << "Created update filemarker '" << metafile.fileName() << "' for downloaded update:" << filePath;

    emit downloadComplete();
    emit installAvailable();
}

void SoftwareUpdate::onDownloadFailed(int id, QString errorMsg) {
    Q_UNUSED(id)
    qCWarning(CLASS_LC) << "Download of update failed:" << errorMsg;

    Notifications::getInstance()->add(true, tr("Download failed: %1").arg(errorMsg));
    emit downloadFailed();
}

bool SoftwareUpdate::installAvailable() { return m_downloadDir.exists(UPDATE_FILEMARKER); }

bool SoftwareUpdate::performAppUpdate() {
    qCDebug(CLASS_LC) << "Executing app update script:" << m_appUpdateScript;

    QProcess process;
    process.setProgram(m_appUpdateScript);
    process.setArguments({m_downloadDir.path() + "/" + UPDATE_FILEMARKER});
    qint64 pid;
    // fire and forget: update script will take over and restart YIO app
    if (process.startDetached(&pid)) {
        qCDebug(CLASS_LC) << "Started update script, pid:" << pid;
        return true;
    } else {
        qCritical(CLASS_LC) << "Failed to start app update script:" << m_appUpdateScript;
        Notifications::getInstance()->add(true, tr("Failed to start app update script!"));
        return false;
    }
}

bool SoftwareUpdate::startDockUpdate() {
    qCWarning(CLASS_LC) << "startDockUpdate() NOT YET IMPLEMENTED";
    return false;
}

bool SoftwareUpdate::isAlreadyDownloaded(const QString &version) {
    QFile metafile(m_downloadDir.path() + "/" + UPDATE_FILEMARKER);

    if (!metafile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&metafile);
    return version.compare(in.readLine(100), Qt::CaseInsensitive) == 0;
}

QString SoftwareUpdate::getDownloadFileName(const QUrl &url) const {
    // attention: download file name contains multiple dots, so we cannot use completeSuffix()!
    // suffix() returns all characters after the LAST dot: https://doc.qt.io/qt-5/qfileinfo.html#suffix
    QString suffix = QFileInfo(url.path()).suffix().toLower();

    // we only support .zip and .tar
    if (suffix == "zip") {
        return UPDATE_BASENAME + ".zip";
    }

    if (suffix == "tar") {
        return UPDATE_BASENAME + ".tar";
    }

    qCWarning(CLASS_LC) << "Download file suffix is neither .zip nor .tar: forcing .tar!" << url.toString();
    return UPDATE_BASENAME + ".tar";
}
