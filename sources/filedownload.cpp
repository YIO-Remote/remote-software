/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2020 Marton Borzak <hello@martonborzak.com>
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

static Q_LOGGING_CATEGORY(CLASS_LC, "filedownload");

// SI or IEC 80000-13 - you choose!
static const int DATA_UNIT = 1000;

FileDownload::FileDownload(QObject *parent) : QObject(parent), m_networkError(QNetworkReply::NetworkError::NoError) {}

FileDownload::~FileDownload() {
    if (m_download) {
        m_download->deleteLater();
    }
}

bool FileDownload::checkDiskSpace(const QDir &path, int requiredMB) {
    QStorageInfo storage = QStorageInfo(path);

    auto availableMB = storage.bytesAvailable() / DATA_UNIT / DATA_UNIT;
    bool valid =
        storage.isValid() && storage.isReady() && !storage.isReadOnly() && availableMB >= requiredMB && path.exists();

    qCDebug(CLASS_LC) << "Storage check for" << requiredMB << "MB:" << valid << "[" << path.path()
                      << ", fileSystemType:" << storage.fileSystemType()
                      << ", size:" << storage.bytesTotal() / DATA_UNIT / DATA_UNIT << "MB"
                      << ", available:" << availableMB << "MB"
                      << ", isReadOnly:" << storage.isReadOnly() << "]";

    if (!valid) {
        m_fileError = QFileDevice::FileError::ResourceError;
        m_errorString = "Storage requirements for download not fulfilled";
    }

    return valid;
}

bool FileDownload::download(const QUrl &downloadUrl, const QDir &destinationDir, const QString &fileName) {
    // TODO(zehnm) check if download is already in progress

    m_fileError = QFileDevice::FileError::NoError;
    m_networkError = QNetworkReply::NetworkError::NoError;
    m_errorString.clear();

    // open the file
    m_file.setFileName(destinationDir.path() + "/" + fileName + ".part");
    if (m_file.exists() && !m_file.remove()) {
        qCCritical(CLASS_LC) << "Error deleting old temporary download file" << m_file << "Error:" << m_file.error()
                             << m_file.errorString();
        return false;
    }
    if (!m_file.open(QIODevice::NewOnly)) {
        qCCritical(CLASS_LC) << "Error opening temporary download file" << m_file << "Error:" << m_file.error()
                             << m_file.errorString();
        return false;
    }
    qCDebug(CLASS_LC) << "File opened";

    // Download into a temp file and rename after successful download

    qCDebug(CLASS_LC) << "Downloading file" << downloadUrl.toString() << "to:" << m_file.fileName();

    // create request
    QNetworkRequest request(downloadUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    m_download = m_manager.get(request);

    connect(m_download, &QNetworkReply::downloadProgress, this, &FileDownload::onDownloadProgress);
    connect(m_download, &QNetworkReply::finished, this, &FileDownload::onDownloadFinished);
    connect(m_download, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this,
            &FileDownload::onDownloadError);

    qCDebug(CLASS_LC) << "Downloading" << downloadUrl.toString();
    // TODO(zehnm) verify if restart() may be called without calling start() first
    m_downloadTimer.restart();

    return true;
}

void FileDownload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    QByteArray  b = m_download->readAll();
    QDataStream out(&m_file);
    out << b;

    qCDebug(CLASS_LC) << "Bytes received:" << bytesReceived << "Bytes total:" << bytesTotal;

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

    emit downloadProgress(bytesReceived, bytesTotal);
    emit downloadSpeed(dowloadSpeed);

    qCDebug(CLASS_LC) << "Download speed:" << dowloadSpeed;
}

void FileDownload::onDownloadError(QNetworkReply::NetworkError error) {
    qCDebug(CLASS_LC) << "Download error:" << error << m_file.errorString();
}

void FileDownload::onDownloadFinished() {
    m_file.close();
    m_networkError = m_download->error();
    m_errorString = m_download->errorString();

    if (m_networkError == QNetworkReply::NetworkError::NoError) {
        // TODO(zehnm) validate download

        QString finalName = m_file.fileName().remove(".part");

        qCDebug(CLASS_LC) << "Download finished. Renaming download file to make install available at:" << finalName;

        if (m_file.rename(finalName)) {
            emit downloadComplete();
        } else {
            m_fileError = m_file.error();
            m_errorString = m_file.errorString();
            qCCritical(CLASS_LC) << "Error renaming download file:" << m_file.error() << m_file.errorString();
            emit downloadFailed();
        }
    } else {
        qCWarning(CLASS_LC) << "Failed to download file:" << m_download->error() << m_download->errorString();
        emit downloadFailed();
    }

    m_download->deleteLater();
}
