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
    qCDebug(CLASS_LC()) << this << "Image provider added:" << m_imageProviderId << m_imageProvider;

    m_startTimer = new QTimer(this);
    m_startTimer->setSingleShot(true);
    m_startTimer->setInterval(1000);

    m_worker       = new MediaPlayerUtilsWorker();
    m_workerThread = new QThread(this);

    QObject::connect(m_worker, &MediaPlayerUtilsWorker::processingDone, this, &MediaPlayerUtils::onProcessingDone);

    m_worker->moveToThread(m_workerThread);
    m_workerThread->start();
}

MediaPlayerUtils::~MediaPlayerUtils() {
    qCDebug(CLASS_LC()) << this << "Destructor called";
    m_engine->removeImageProvider(m_imageProviderId);
    qCDebug(CLASS_LC()) << this << "Destructor: Image provider removed:" << m_imageProviderId;

    if (m_worker != nullptr) {
        qCDebug(CLASS_LC()) << this << "Destructor: Worker was not null";
        m_worker->disconnect();
        qCDebug(CLASS_LC()) << this << "Destructor: Signal disconnected from Worker class";
        m_worker->deleteLater();
        m_worker = nullptr;
        qCDebug(CLASS_LC()) << this << "Destructor: Worker deleted";
    }

    if (m_workerThread != nullptr) {
        qCDebug(CLASS_LC()) << this << "Destructor: WorkerThread was not null";
        if (m_workerThread->isRunning()) {
            qCDebug(CLASS_LC()) << this << "Destructor: WorkerThread is running";
            m_workerThread->quit();
            qCDebug(CLASS_LC()) << this << "Destructor: WorkerThread exit was called";
            m_workerThread->wait();
            qCDebug(CLASS_LC()) << this << "Destructor: WorkerThread wait";
        }
        m_workerThread->deleteLater();
        m_workerThread = nullptr;
        qCDebug(CLASS_LC()) << this << "Destructor: WorkerThread was deleted";
    }

    if (m_startTimer->isActive()) {
        m_startTimer->stop();
    }
    m_startTimer->deleteLater();
    m_startTimer = nullptr;
    m_engine     = nullptr;
    m_engine->deleteLater();
    m_imageProvider = nullptr;
    m_imageProvider->deleteLater();

    qCDebug(CLASS_LC()) << this << "Destructor end";
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

    qCDebug(CLASS_LC()) << this << "Processing done";
}

void MediaPlayerUtils::generateImages(const QString &url) {
    if (url != m_prevImageURL) {
        qCDebug(CLASS_LC()) << this << "Generate images for" << url;

        m_prevImageURL = url;
        emit processingStarted();

        m_worker->generateImages(url);
    }
}

MediaPlayerUtilsWorker::MediaPlayerUtilsWorker(QObject *parent) : QObject(parent) {
    m_manager = new QNetworkAccessManager();
}

MediaPlayerUtilsWorker::~MediaPlayerUtilsWorker() {
    qCDebug(CLASS_LC()) << this << "Worker destructor";
    if (m_manager != nullptr) {
        m_manager->deleteLater();
        m_manager = nullptr;
        qCDebug(CLASS_LC()) << this << "Worker destructor: Manager deleted.";
    }
}

void MediaPlayerUtilsWorker::generateImages(const QString &url) {
    m_reply = m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_reply, &QNetworkReply::finished, this, &MediaPlayerUtilsWorker::generateImagesReply);
}

void MediaPlayerUtilsWorker::generateImagesReply() {
    if (m_reply != nullptr) {
        m_reply->disconnect();

        if (m_reply->error() == QNetworkReply::NoError) {
            // run image processing in different thread
            QImage image(280, 280, QImage::Format_RGB888);
            image.fill(Qt::black);

            if (!image.load(m_reply, nullptr)) {
                qCWarning(CLASS_LC) << "ERROR LOADING IMAGE";
                if (m_reply) {
                    m_reply->deleteLater();
                    m_reply = nullptr;
                }
                return;
            } else {
                ////////////////////////////////////////////////////////////////////
                /// GET DOMINANT COLOR
                ////////////////////////////////////////////////////////////////////
                qCDebug(CLASS_LC()) << this << "Getting dominant color";
                QColor pixelColor = dominantColor(image);

                // change the brightness of the color if it's too bright
                if (pixelColor.lightness() > 150) {
                    pixelColor.setHsv(pixelColor.hue(), pixelColor.saturation(), (pixelColor.value() - 80));
                }

                // if the color is close to white, return black instead
                if (pixelColor.lightness() > 210) {
                    pixelColor = QColor("black");
                }

                ////////////////////////////////////////////////////////////////////
                /// CREATE LARGE BACKGROUND IMAGE
                ////////////////////////////////////////////////////////////////////
                qCDebug(CLASS_LC()) << this << "Creating image";
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

                qCDebug(CLASS_LC()) << this << "Creating image DONE";

                emit processingDone(pixelColor, image);
            }
        } else {
            qCWarning(CLASS_LC) << this << "NETWORK REPLY ERROR" << m_reply->errorString();
            emit processingDone(QColor("black"), QImage());
        }

        m_reply->deleteLater();
        m_reply = nullptr;
        qCDebug(CLASS_LC()) << this << "Network reply deleted";
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IMAGE PROVIDER
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MediaPlayerUtilsImageProvider::MediaPlayerUtilsImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
    m_noImage = QImage(280, 280, QImage::Format_ARGB32);
    m_noImage.fill(qRgba(0, 0, 0, 0));
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
