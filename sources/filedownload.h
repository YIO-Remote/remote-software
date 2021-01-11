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

#pragma once

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QQueue>

/**
 * @brief FileDownload allows to download binary files from the web into local files.
 * Multiple downloads are queued and executed sequentially.
 * An individual download is asynchrounous and chunked, i.e. the data is not completely read into memory but downloaded
 * in chunks.
 */
class FileDownload : public QObject {
    Q_OBJECT

 public:
    explicit FileDownload(QObject *parent = nullptr);

    ~FileDownload();

    /**
     * @brief Check if there's enough free disk space and if the download location is writable.
     * @param path Download location
     * @param requiredMB Minimum of free disk space in megabyte
     * @return true if download location is usable, false otherwise
     */
    bool checkDiskSpace(const QDir &path, int requiredMB);

    /**
     * @brief Download the given file in downloadURL and store it in the given local file.
     * The download is queued and each download is processed sequentially. Download feedback is given through the public
     * signals.
     * @param downloadUrl The download URL
     * @param destinationDir The destination directory where to store the file
     * @param fileName The filename to use for the download
     * @param requiredFreeMB Check if there's enough free disk space in megabyte if value > 0
     * @return Download identifier used for the signals
     */
    int download(const QUrl &downloadUrl, const QDir &destinationDir, const QString &fileName, int requiredFreeMB = 0);

 signals:
    /**
     * @brief This signal is emitted to indicate the progress of the current download.
     * @param id The download identifier
     * @param bytesReceived The number of bytes received
     * @param bytesTotal Total number of bytes expected to be downloaded
     * @param speed A human readable download speed label, e.g. "1.2 MB/s"
     * @see https://doc.qt.io/qt-5/qnetworkreply.html#downloadProgress
     */
    void downloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal, const QString &speed);

    /**
     * @brief This signal is emitted after a download has been successfully downloaded.
     * @param filePath The downloaded filename including path
     * @param id The download identifier
     */
    void downloadComplete(int id, const QString &filePath);

    /**
     * @brief This signal is emitted if a download failed.
     * @param id The download identifier
     * @param errorMsg A human-readable description of the error
     */
    void downloadFailed(int id, QString errorMsg);

    /**
     * @brief Signals that the last download has been finished and that there are no more downloads in the queue.
     */
    void downloadQueueEmpty();

 private slots:
    void startNextDownload();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadError(QNetworkReply::NetworkError error);
    void onDownloadFinished();

 private:
    struct Download {
     public:
        int     id;
        QUrl    url;
        QDir    destinationDir;
        QString fileName;
        int     requiredMb;
    };

    bool prepareFileDownload(const Download &download);

    int                   m_downloadId = 0;
    int                   m_currentDownloadId = 0;
    QNetworkAccessManager m_manager;
    QQueue<Download>      m_downloadQueue;
    QFile                 m_outputFile;
    QNetworkReply *       m_currentReply = nullptr;
    QElapsedTimer         m_downloadTimer;
};
