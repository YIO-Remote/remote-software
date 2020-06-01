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
#include <QQmlApplicationEngine>
#include <QQuickImageProvider>
#include <QThread>
#include <QTimer>
#include <QUuid>

class MediaPlayerUtilsImageProvider : public QObject, public QQuickImageProvider {
    Q_OBJECT

 public:
    explicit MediaPlayerUtilsImageProvider();

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void updateImage(const QImage& image);

 signals:
    void imageChanged();

 private:
    QImage m_image;
    QImage m_noImage;
};

class MediaPlayerUtilsWorker : public QObject {
    Q_OBJECT

 public:
    explicit MediaPlayerUtilsWorker(QObject* parent = nullptr);
    ~MediaPlayerUtilsWorker();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void generateImages(const QString& url);
    void generateImagesReply();

 signals:
    void processingDone(const QColor& pixelColor, const QImage& image);

 private:
    QNetworkAccessManager* m_manager;
    QNetworkReply*         m_reply;
    QColor                 dominantColor(const QImage& image);
};

class MediaPlayerUtils : public QObject {
    Q_OBJECT

 public:
    explicit MediaPlayerUtils(QObject* parent = nullptr);
    ~MediaPlayerUtils();

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString imageURL READ imageURL WRITE setImageURL)
    Q_PROPERTY(QString imageProviderId READ imageProviderId CONSTANT)
    Q_PROPERTY(QColor pixelColor READ pixelColor NOTIFY pixelColorChanged)

    bool    enabled() { return m_enabled; }
    QString imageURL() { return m_imageURL; }
    QString imageProviderId() { return m_imageProviderId; }
    QColor  pixelColor() { return m_pixelColor; }

    void setImageURL(QString url);
    void setEnabled(bool value);

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onProcessingDone(const QColor& pixelColor, const QImage& image);

 signals:
    void processingStarted();
    void enabledChanged();
    void imageChanged();
    void pixelColorChanged();

 private:
    bool m_enabled = true;

    QString m_imageURL;
    QString m_prevImageURL;
    QColor  m_pixelColor;
    QTimer* m_startTimer;

    void generateImages(const QString& url);

    QThread*                       m_workerThread = nullptr;
    MediaPlayerUtilsWorker*        m_worker       = nullptr;
    MediaPlayerUtilsImageProvider* m_imageProvider;
    QString                        m_imageProviderId;

    QQmlApplicationEngine* m_engine;
};
