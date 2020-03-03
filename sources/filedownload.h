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

#pragma once

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class FileDownload : public QObject {
    Q_OBJECT

 public:
    explicit FileDownload(QObject *parent = nullptr);

    ~FileDownload();

    bool checkDiskSpace(const QDir &path, int requiredMB);
    bool download(const QUrl &downloadUrl, const QDir &destinationDir, const QString &fileName);

    bool                        hasError() const { return m_fileError != 0 || m_networkError != 0; }
    QFileDevice::FileError      fileError() const { return m_fileError; }
    QNetworkReply::NetworkError networkError() const { return m_networkError; }
    QString                     errorString() const { return m_errorString; }

 signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadSpeed(const QString &speed);
    void downloadComplete();
    void downloadFailed();

 private slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadError(QNetworkReply::NetworkError error);
    void onDownloadFinished();

 private:
    QNetworkAccessManager m_manager;
    QFile                 m_file;
    QNetworkReply *       m_download = nullptr;
    QElapsedTimer         m_downloadTimer;

    QFileDevice::FileError      m_fileError;
    QNetworkReply::NetworkError m_networkError;
    QString                     m_errorString;
};
