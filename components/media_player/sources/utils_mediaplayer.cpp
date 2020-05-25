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

#include "utils_mediaplayer.h"

#include <QDebug>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(CLASS_LC, "mediaplayer utils");

MediaPlayerUtils::MediaPlayerUtils() {}

MediaPlayerUtils::~MediaPlayerUtils() {
    if (m_workerThread->isRunning()) {
        qCDebug(CLASS_LC()) << "Destructor: Thread is running. Quitting.";
        m_workerThread->quit();
        if (!m_workerThread->wait(3000)) {
            qCWarning(CLASS_LC()) << "Destructor: Thread didn't quit. Terminating.";
            m_workerThread->terminate();
            m_workerThread->wait();
            qCWarning(CLASS_LC()) << "Destructor: Thread terminated.";
            m_workerThread->deleteLater();
            m_workerThread = nullptr;
            qCDebug(CLASS_LC()) << "Destructor: Thread removed and deleted";
        }
    }
    if (m_worker) {
        m_worker->deleteLater();
        m_worker = nullptr;
        qCDebug(CLASS_LC()) << "Destructor: Worker class deleted";
    }
    if (m_manager) {
        m_manager->deleteLater();
        m_manager = nullptr;
        qCDebug(CLASS_LC()) << "Destructor: QNetworkAccessManager deleted";
    }
}

void MediaPlayerUtils::setImageURL(QString url) {
    m_imageURL = url;

    if (m_imageURL != m_prevImageURL && m_enabled && !m_imageURL.isEmpty()) {
        generateImages(m_imageURL);
    }
}

void MediaPlayerUtils::setEnabled(bool value) {
    if (m_enabled != value) {
        m_enabled = value;
        emit enabledChanged();

        if (m_enabled && !m_imageURL.isEmpty()) {
            generateImages(m_imageURL);
        }
    }
}

void MediaPlayerUtils::onProcessingDone(const QColor &pixelColor, const QString &smallImage,
                                        const QString &largeImage) {
    m_pixelColor = pixelColor;
    emit pixelColorChanged();

    m_smallImage = smallImage;
    emit smallImageChanged();

    m_image = largeImage;
    emit imageChanged();

    if (m_workerThread->isRunning()) {
        qCDebug(CLASS_LC()) << "Thread is running. Quitting.";
        m_workerThread->quit();
        if (!m_workerThread->wait(3000)) {
            qCWarning(CLASS_LC()) << "Thread didn't quit. Terminating.";
            m_workerThread->terminate();
            m_workerThread->wait();
            qCWarning(CLASS_LC()) << "Thread terminated.";
            m_workerThread->deleteLater();
            m_workerThread = nullptr;
            qCDebug(CLASS_LC()) << "Thread removed and deleted";
        }
    }
    m_worker->deleteLater();
    m_worker = nullptr;
    qCDebug(CLASS_LC()) << "Worker class deleted";
    m_manager->deleteLater();
    m_manager = nullptr;
    qCDebug(CLASS_LC()) << "QNetworkAccessManager deleted";
}

void MediaPlayerUtils::generateImages(const QString &url) {
    if (url != m_prevImageURL) {
        m_prevImageURL = url;
        emit processingStarted();
        m_worker = new MediaPlayerUtilsWorker();

        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, &QNetworkAccessManager::finished, m_worker, &MediaPlayerUtilsWorker::generateImagesReply);

        m_workerThread = new QThread();
        connect(m_worker, &MediaPlayerUtilsWorker::processingDone, this, &MediaPlayerUtils::onProcessingDone);
        m_worker->moveToThread(m_workerThread);
        m_workerThread->start();

        m_manager->get(QNetworkRequest(QUrl(url)));
    }
}

void MediaPlayerUtilsWorker::generateImagesReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        // run image processing in different thread
        QImage image;

        if (!image.load(reply, nullptr)) {
            qCWarning(CLASS_LC) << "ERROR LOADING IMAGE";
            return;
        } else {
            ////////////////////////////////////////////////////////////////////
            /// GET DOMINANT COLOR
            ////////////////////////////////////////////////////////////////////
            qCDebug(CLASS_LC()) << "Getting dominant color";
            //            QColor m_pixelColor = dominantColor(image);

            // shrink down image to 1x1 pixel and measure the color
            QImage onePixel = image;
            onePixel.scaled(1, 1, Qt::IgnoreAspectRatio);
            QColor m_pixelColor = onePixel.pixel(1, 1);

            // change the brightness of the color if it's too bright
            if (m_pixelColor.lightness() > 150) {
                m_pixelColor.setHsv(m_pixelColor.hue(), m_pixelColor.saturation(), (m_pixelColor.value() - 80));
            }

            // if the color is close to white, return black instead
            if (m_pixelColor.lightness() > 210) {
                m_pixelColor = QColor("black");
            }

            ////////////////////////////////////////////////////////////////////
            /// CREATE A SMALL THUMBNAIL IMAGE
            ////////////////////////////////////////////////////////////////////
            qCDebug(CLASS_LC()) << "Creating small image";
            QImage smallImage = image;
            smallImage.scaledToHeight(90, Qt::SmoothTransformation);

            // create byte array and then convert to base64
            QByteArray bArray;
            QBuffer    buffer(&bArray);
            buffer.open(QIODevice::WriteOnly);
            smallImage.save(&buffer, "JPEG");

            QString bImage("data:image/jpg;base64,");
            bImage.append(QString::fromLatin1(bArray.toBase64().data()));

            QString m_smallImage = bImage;
            qCDebug(CLASS_LC()) << "Creating small image DONE";

            ////////////////////////////////////////////////////////////////////
            /// CREATE LARGE BACKGROUND IMAGE
            ////////////////////////////////////////////////////////////////////
            qCDebug(CLASS_LC()) << "Creating large image";
            // resize image
            image.scaledToHeight(280, Qt::SmoothTransformation);

            // create noise layer
            QImage noise(":/images/mini-music-player/noise.png");
            noise.scaledToHeight(280, Qt::SmoothTransformation);
            noise = noise.convertToFormat(QImage::Format_ARGB32);

            // merge the images together
            QPainter painter(&image);
            painter.drawImage(image.rect(), noise);
            painter.end();

            // create byte array and then convert to base64
            QByteArray lArray;
            QBuffer    lBuffer(&lArray);
            lBuffer.open(QIODevice::WriteOnly);
            image.save(&lBuffer, "JPEG");

            QString lImage("data:image/jpg;base64,");
            lImage.append(QString::fromLatin1(lArray.toBase64().data()));

            QString m_largeImage = lImage;
            qCDebug(CLASS_LC()) << "Creating large image DONE";

            emit processingDone(m_pixelColor, m_smallImage, m_largeImage);
            if (reply) {
                reply->deleteLater();
                reply = nullptr;
            }
            qCDebug(CLASS_LC()) << "Network reply deleted";
        }
    } else {
        qCWarning(CLASS_LC) << "NETWORK REPLY ERROR" << reply->errorString();
        emit processingDone(QColor("black"), "", "");
        return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DOMINANT COLOR OF IMAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QColor MediaPlayerUtilsWorker::dominantColor(const QImage &image) {
    qint32 averageRed = 0, averageGreen = 0, averageBlue = 0;

    const qint32 maxX = qMin<qint32>(image.width(), 0 + image.width());
    const qint32 maxY = qMin<qint32>(image.height(), 0 + image.height());

    for (qint32 y = 0; y < maxY; y++) {
        for (qint32 x = 0; x < maxX; x++) {
            QRgb pixel = image.pixel(x, y);

            averageRed += qRed(pixel);
            averageGreen += qGreen(pixel);
            averageBlue += qBlue(pixel);
        }
    }

    qint32 n = image.width() * image.height();

    if (n <= 0) {
        return Qt::black;
    } else {
        return QColor(averageRed / n, averageGreen / n, averageBlue / n);
    }
}
