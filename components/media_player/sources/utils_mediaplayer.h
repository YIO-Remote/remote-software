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

#pragma once

#include <QBuffer>
#include <QColor>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPainter>
#include <QThread>

class MediaPlayerUtilsWorker : public QObject {
    Q_OBJECT

 public:
    MediaPlayerUtilsWorker() {}
    virtual ~MediaPlayerUtilsWorker() {}

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void generateImagesReply(QNetworkReply* reply);

 signals:
    void processingDone(const QColor& pixelColor, const QString& smallImage, const QString& largeImage);

 private:
    QColor dominantColor(const QImage& image);
};

class MediaPlayerUtils : public QObject {
    Q_OBJECT

 public:
    MediaPlayerUtils();
    virtual ~MediaPlayerUtils();

    Q_PROPERTY(QString imageURL READ imageURL WRITE setImageURL)

    QString imageURL() { return m_imageURL; }

    void setImageURL(QString url) {
        m_imageURL = url;
        generateImages(url);
    }

    Q_PROPERTY(QString image READ image NOTIFY imageChanged)

    QString image() { return m_image; }

    Q_PROPERTY(QString smallImage READ smallImage NOTIFY smallImageChanged)

    QString smallImage() { return m_smallImage; }

    Q_PROPERTY(QColor pixelColor READ pixelColor NOTIFY pixelColorChanged)

    QColor pixelColor() { return m_pixelColor; }

 public slots:
    void onProcessingDone(const QColor& pixelColor, const QString& smallImage, const QString& largeImage);

 signals:
    void imageChanged();
    void smallImageChanged();
    void pixelColorChanged();

 private:
    QString m_imageURL;
    QString m_image;
    QString m_smallImage;
    QColor  m_pixelColor;

    void generateImages(const QString& url);

    QNetworkAccessManager*  m_manager;
    QThread*                m_workerThread;
    MediaPlayerUtilsWorker* m_worker;
};
