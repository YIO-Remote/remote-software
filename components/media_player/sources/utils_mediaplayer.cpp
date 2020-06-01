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
#include "../sources/config.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "mediaplayer utils");

MediaPlayerUtils::MediaPlayerUtils(QObject *parent) : QObject(parent) {
    // ADD IMAGE PROVIDER
    m_engine = Config::getInstance()->getAppEngine();

    m_imageProviderId = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    m_imageProvider   = new MediaPlayerUtilsImageProvider();
    m_engine->addImageProvider(m_imageProviderId, m_imageProvider);
    qCDebug(CLASS_LC()) << "Image provider added:" << m_imageProviderId << m_imageProvider;

    m_startTimer = new QTimer(this);
    m_startTimer->setSingleShot(true);
    m_startTimer->setInterval(1000);
}

MediaPlayerUtils::~MediaPlayerUtils() {
    qCDebug(CLASS_LC()) << "Destructor called";
    m_engine->removeImageProvider(m_imageProviderId);
    qCDebug(CLASS_LC()) << "Image provider removed:" << m_imageProviderId;

    if (m_worker != nullptr) {
        m_worker->disconnect();
        qCDebug(CLASS_LC()) << "Destructor: signal disconnected from Worker class";
    }

    if (m_workerThread != nullptr) {
        if (m_workerThread->isRunning()) {
            qCDebug(CLASS_LC()) << "Destructor: Worker thread is running";
            m_workerThread->exit();
            if (!m_workerThread->wait(5000)) {
                qCDebug(CLASS_LC()) << "Destructor: Terminating worker thread";
                m_workerThread->terminate();
                m_workerThread->wait();
                m_worker = nullptr;
                qCDebug(CLASS_LC()) << "Destructor: Worker class deleted";
                m_workerThread = nullptr;
                qCDebug(CLASS_LC()) << "Destructor: Worker thread deleted";
            } else {
                m_worker = nullptr;
                qCDebug(CLASS_LC()) << "Destructor: Worker class deleted";
                m_workerThread = nullptr;
                qCDebug(CLASS_LC()) << "Destructor: Worker thread deleted";
            }
        } else {
            m_worker = nullptr;
            qCDebug(CLASS_LC()) << "Destructor: Worker class deleted";
            m_workerThread = nullptr;
            qCDebug(CLASS_LC()) << "Destructor: Worker thread deleted";
        }
    }

    m_startTimer    = nullptr;
    m_engine        = nullptr;
    m_imageProvider = nullptr;

    qCDebug(CLASS_LC()) << "Destructor end";
}

void MediaPlayerUtils::setImageURL(QString url) {
    m_imageURL = url;

    if (m_imageURL != m_prevImageURL && m_enabled && !m_imageURL.isEmpty()) {
        if (m_startTimer->isActive()) {
            m_startTimer->stop();
        }
        QObject::connect(m_startTimer, &QTimer::timeout, this, [=] { generateImages(m_imageURL); });
        m_startTimer->start();
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

void MediaPlayerUtils::onProcessingDone(const QColor &pixelColor, const QImage &image) {
    m_imageProvider->updateImage(image);
    emit imageChanged();

    m_pixelColor = pixelColor;
    emit pixelColorChanged();

    if (m_worker != nullptr) {
        m_worker->disconnect();
        qCDebug(CLASS_LC()) << "Signal disconnected from Worker class";
    }

    if (m_workerThread != nullptr) {
        if (m_workerThread->isRunning()) {
            qCDebug(CLASS_LC()) << "Worker thread is running";
            m_workerThread->exit();
            if (!m_workerThread->wait(5000)) {
                qCDebug(CLASS_LC()) << "Terminating worker thread";
                m_workerThread->terminate();
                m_workerThread->wait();
                m_worker = nullptr;
                qCDebug(CLASS_LC()) << "Worker class deleted";
                m_workerThread = nullptr;
                qCDebug(CLASS_LC()) << "Worker thread deleted";
            } else {
                m_worker = nullptr;
                qCDebug(CLASS_LC()) << "Worker class deleted";
                m_workerThread = nullptr;
                qCDebug(CLASS_LC()) << "Worker thread deleted";
            }
        } else {
            m_worker = nullptr;
            qCDebug(CLASS_LC()) << "Worker class deleted";
            m_workerThread = nullptr;
            qCDebug(CLASS_LC()) << "Worker thread deleted";
        }
    }
}

void MediaPlayerUtils::generateImages(const QString &url) {
    qCDebug(CLASS_LC()) << "Generate images for" << url;
    if (url != m_prevImageURL) {
        m_prevImageURL = url;
        emit processingStarted();

        m_worker       = new MediaPlayerUtilsWorker();
        m_workerThread = new QThread(this);

        connect(m_worker, &MediaPlayerUtilsWorker::processingDone, this, &MediaPlayerUtils::onProcessingDone);

        m_worker->moveToThread(m_workerThread);
        m_workerThread->start();
        m_worker->generateImages(url);
    }
}

MediaPlayerUtilsWorker::MediaPlayerUtilsWorker(QObject *parent) : QObject(parent) {}

MediaPlayerUtilsWorker::~MediaPlayerUtilsWorker() {
    qCDebug(CLASS_LC()) << "Worker destructor";
    m_manager = nullptr;
    qCDebug(CLASS_LC()) << "Worker destructor: Manager deleted.";
}

void MediaPlayerUtilsWorker::generateImages(const QString &url) {
    m_manager = new QNetworkAccessManager();
    m_reply   = m_manager->get(QNetworkRequest(QUrl(url)));

    connect(m_reply, &QNetworkReply::finished, this, &MediaPlayerUtilsWorker::generateImagesReply);
}

void MediaPlayerUtilsWorker::generateImagesReply() {
    if (m_reply->error() == QNetworkReply::NoError) {
        // run image processing in different thread
        QImage image;

        if (!image.load(m_reply, nullptr)) {
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
            /// CREATE LARGE BACKGROUND IMAGE
            ////////////////////////////////////////////////////////////////////
            qCDebug(CLASS_LC()) << "Creating image";
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

            qCDebug(CLASS_LC()) << "Creating image DONE";

            emit processingDone(m_pixelColor, image);
        }
    } else {
        qCWarning(CLASS_LC) << "NETWORK REPLY ERROR" << m_reply->errorString();
        emit processingDone(QColor("black"), QImage());
    }
    m_reply->deleteLater();
    qCDebug(CLASS_LC()) << "Network reply deleted";
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IMAGE PROVIDER
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MediaPlayerUtilsImageProvider *MediaPlayerUtilsImageProvider::s_instance = nullptr;

MediaPlayerUtilsImageProvider::MediaPlayerUtilsImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
    s_instance = this;

    m_noImage = QImage();
    blockSignals(false);
}

QImage MediaPlayerUtilsImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id)

    QImage result = m_image;

    if (result.isNull()) {
        result = m_noImage;
    }

    if (size) {
        *size = result.size();
    }

    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        result = result.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
    }

    return result;
}

void MediaPlayerUtilsImageProvider::updateImage(const QImage &image) {
    if (m_image != image) {
        m_image = image;
        emit imageChanged();
    }
}
