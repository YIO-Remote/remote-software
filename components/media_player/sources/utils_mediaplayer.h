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
    virtual ~MediaPlayerUtilsWorker();

    void terminateWork();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void generateImages(const QString& url);
    void generateImagesReply();

 signals:
    void processingDone(const QColor& pixelColor, const QString& smallImage, const QString& largeImage);

 private:
    QNetworkAccessManager* m_manager;
    QNetworkReply*         m_reply;
    QColor                 dominantColor(const QImage& image);
};

class MediaPlayerUtils : public QObject {
    Q_OBJECT

 public:
    MediaPlayerUtils();
    virtual ~MediaPlayerUtils();

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString imageURL READ imageURL WRITE setImageURL)
    Q_PROPERTY(QString image READ image NOTIFY imageChanged)
    Q_PROPERTY(QString smallImage READ smallImage NOTIFY smallImageChanged)
    Q_PROPERTY(QColor pixelColor READ pixelColor NOTIFY pixelColorChanged)

    bool    enabled() { return m_enabled; }
    QString imageURL() { return m_imageURL; }
    QString image() { return m_image; }
    QString smallImage() { return m_smallImage; }
    QColor  pixelColor() { return m_pixelColor; }

    void setImageURL(QString url);
    void setEnabled(bool value);

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onProcessingDone(const QColor& pixelColor, const QString& smallImage, const QString& largeImage);

 signals:
    void processingStarted();
    void enabledChanged();
    void imageChanged();
    void smallImageChanged();
    void pixelColorChanged();

 private:
    bool m_enabled = true;

    QString m_imageURL;
    QString m_prevImageURL;
    QString m_image;
    QString m_smallImage;
    QColor  m_pixelColor;

    void generateImages(const QString& url);

    QThread*                m_workerThread;
    MediaPlayerUtilsWorker* m_worker;
};
