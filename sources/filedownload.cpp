/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#include "filedownload.h"

#include <QDataStream>
#include <QDebug>
#include <QLoggingCategory>
#include <QStorageInfo>
#include <QTimer>

static Q_LOGGING_CATEGORY(CLASS_LC, "filedownload");

// SI or IEC 80000-13 - you choose!
static const int DATA_UNIT = 1000;

FileDownload::FileDownload(QObject *parent) : QObject(parent) {}

FileDownload::~FileDownload() {
    if (m_currentReply) {
        m_currentReply->deleteLater();
    }
}

bool FileDownload::checkDiskSpace(const QDir &path, int requiredMB) {
    QStorageInfo storage = QStorageInfo(path);

    if (requiredMB <= 0) {
        qCDebug(CLASS_LC) << "Storage check is disabled";
        return true;
    }

    auto availableMB = storage.bytesAvailable() / DATA_UNIT / DATA_UNIT;
    // Note: storage.isReadOnly() doesn't work on macOS Catalina. Maybe in a newer Qt version?
    bool valid = storage.isValid() && storage.isReady() &&  // && !storage.isReadOnly()
                 availableMB >= requiredMB && path.exists();

    qCDebug(CLASS_LC) << "Storage check for" << requiredMB << "MB:" << valid << "[" << path.path()
                      << ", fileSystemType:" << storage.fileSystemType()
                      << ", size:" << storage.bytesTotal() / DATA_UNIT / DATA_UNIT << "MB"
                      << ", available:" << availableMB << "MB"
                      << ", isReadOnly:" << storage.isReadOnly() << "]";

    return valid;
}

int FileDownload::download(const QUrl &downloadUrl, const QDir &destinationDir, const QString &fileName,
                           int requiredFreeMB /* = 0 */) {
    if (m_downloadQueue.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(startNextDownload()));
    }

    m_downloadId++;
    m_downloadQueue.enqueue({m_downloadId, downloadUrl, destinationDir, fileName, requiredFreeMB});
    qCDebug(CLASS_LC) << "Enqueued download:" << m_downloadId << downloadUrl.toString();

    return m_downloadId;
}

void FileDownload::startNextDownload() {
    if (m_downloadQueue.isEmpty()) {
        qCDebug(CLASS_LC) << "Finished, no more files to download";
        emit downloadQueueEmpty();
        return;
    }

    Download download = m_downloadQueue.dequeue();
    m_currentDownloadId = download.id;
    qCDebug(CLASS_LC) << "Starting next download:" << download.id << download.url.toString();

    // check local preconditions
    if (!prepareFileDownload(download)) {
        QTimer::singleShot(0, this, SLOT(startNextDownload()));
        return;  // skip this download
    }

    qCDebug(CLASS_LC) << "Downloading file from" << download.url.toString() << "to:" << m_outputFile.fileName();

    // Download into a temp file and rename after successful download
    QNetworkRequest request(download.url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    m_currentReply = m_manager.get(request);

    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &FileDownload::onDownloadProgress);
    connect(m_currentReply, &QNetworkReply::finished, this, &FileDownload::onDownloadFinished);
    connect(m_currentReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &FileDownload::onDownloadError);

    m_downloadTimer.restart();
}

bool FileDownload::prepareFileDownload(const Download &download) {
    if (!checkDiskSpace(download.destinationDir, download.requiredMb)) {
        emit downloadFailed(download.id, tr("Not enough free space (%1 MB).").arg(download.requiredMb));
        return false;
    }

    QFile finalFile(download.destinationDir.path() + "/" + download.fileName);
    if (finalFile.exists()) {
        qCCritical(CLASS_LC) << "Destination file already exists:" << finalFile.fileName();
        emit downloadFailed(download.id, tr("Destination file already exists"));
        return false;
    }

    bool fileError = false;
    m_outputFile.setFileName(finalFile.fileName() + ".part");

    if (m_outputFile.exists() && !m_outputFile.remove()) {
        fileError = true;
    } else if (!m_outputFile.open(QIODevice::NewOnly)) {
        fileError = true;
    }

    if (fileError) {
        qCCritical(CLASS_LC) << "Error opening temporary download file" << m_outputFile
                             << "Error:" << m_outputFile.error() << m_outputFile.errorString();
        emit downloadFailed(download.id, m_outputFile.errorString());
    }

    return !fileError;
}

void FileDownload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    QByteArray  b = m_currentReply->readAll();
    QDataStream out(&m_outputFile);
    out << b;

    QString dowloadSpeed;
    if (bytesReceived > 0) {
        double  speed = bytesReceived * 1000 / m_downloadTimer.elapsed();
        QString unit;
        if (speed < DATA_UNIT) {
            unit = " B/s";
        } else if (speed < DATA_UNIT * DATA_UNIT) {
            speed /= DATA_UNIT;
            unit = " kB/s";
        } else {
            speed /= DATA_UNIT * DATA_UNIT;
            unit = " MB/s";
        }

        dowloadSpeed = QString::number(speed, 'f', 1);
        dowloadSpeed.append(unit);
    } else {
        dowloadSpeed = "0 B/s";
    }

    qCDebug(CLASS_LC) << "Bytes received:" << bytesReceived << "Bytes total:" << bytesTotal
                      << "Elapsed ms:" << m_downloadTimer.elapsed() << "Download speed:" << dowloadSpeed;

    emit downloadProgress(m_currentDownloadId, bytesReceived, bytesTotal, dowloadSpeed);
}

void FileDownload::onDownloadError(QNetworkReply::NetworkError error) {
    qCDebug(CLASS_LC) << "Download error:" << error << m_outputFile.errorString();
}

void FileDownload::onDownloadFinished() {
    m_outputFile.close();

    qCDebug(CLASS_LC) << "Elapsed ms:" << m_downloadTimer.elapsed();

    if (m_currentReply->error() == QNetworkReply::NetworkError::NoError) {
        // TODO(zehnm) validate download

        QString finalName = m_outputFile.fileName().remove(".part");

        qCDebug(CLASS_LC) << "Download finished. Renaming download file to make install available at:" << finalName;

        if (m_outputFile.rename(finalName)) {
            emit downloadComplete(m_currentDownloadId, m_outputFile.fileName());
        } else {
            qCCritical(CLASS_LC) << "Error renaming download file:" << m_outputFile.error()
                                 << m_outputFile.errorString();
            emit downloadFailed(m_currentDownloadId, m_outputFile.errorString());
        }
    } else {
        qCWarning(CLASS_LC) << "Failed to download file:" << m_currentReply->error() << m_currentReply->errorString();
        emit downloadFailed(m_currentDownloadId, m_currentReply->errorString());
    }

    m_currentReply->deleteLater();
    startNextDownload();
}
