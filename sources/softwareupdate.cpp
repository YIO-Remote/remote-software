#include "softwareupdate.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "softwareupdate");

SoftwareUpdate *SoftwareUpdate::s_instance = nullptr;

SoftwareUpdate::SoftwareUpdate(bool autoUpdate, QObject *parent) : QObject(parent), m_autoUpdate(autoUpdate) {
    s_instance = this;

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
    connect(m_manager, &QNetworkAccessManager::finished, this, &SoftwareUpdate::checkForUpdateFinished);

    m_updateUrl.append("/?curr_version=").append(m_currentVersion);
    m_manager->get(QNetworkRequest(QUrl(m_updateUrl)));
}

void SoftwareUpdate::checkForUpdateFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        // process reply here
        // if update is available
        // update m_downloadUrl
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

        // else
        m_updateAvailable = false;
        emit updateAvailableChanged();
    } else {
        qCWarning(CLASS_LC) << "Network reply error: " << reply->errorString();
        Notifications::getInstance()->add(true, "Cannot connect to the update server.");
    }

    reply->deleteLater();
    qCDebug(CLASS_LC) << "Network reply deleted";

    m_manager->deleteLater();
    qCDebug(CLASS_LC) << "Networkaccessmanager deleted";
}

void SoftwareUpdate::downloadUpdate(QUrl url) {
    // open the file
    m_file = new QFile(m_destination);
    m_file->open(QIODevice::WriteOnly);
    qCDebug(CLASS_LC) << "File opened";

    // create network manager
    m_manager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    m_download = m_manager->get(request);

    connect(m_download, &QNetworkReply::downloadProgress, this, &SoftwareUpdate::downloadProgress);
    connect(m_download, &QNetworkReply::finished, this, &SoftwareUpdate::downloadFinished);

    qCDebug(CLASS_LC) << "Downloading" << url.toEncoded().constData();
    m_downloadTimer->start();
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

    double  speed = bytesReceived * 1000.0 / m_downloadTimer->elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024 * 1014;
        unit = "MB/s";
    }

    m_downloadSpeed = QString::number(speed);
    m_downloadSpeed.append(unit);
    emit downloadSpeedChanged();
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
}

void SoftwareUpdate::startUpdate() {
    // start software update procedure
    downloadUpdate(m_downloadUrl);
}

void SoftwareUpdate::startDockUpdate() {}
