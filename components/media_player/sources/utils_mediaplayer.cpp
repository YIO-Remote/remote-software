#include "utils_mediaplayer.h"

#include <QDebug>

void MediaPlayerUtils::generateImages(QString url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MediaPlayerUtils::generateImagesReply);
    manager->get(QNetworkRequest(QUrl(url)));
}

void MediaPlayerUtils::generateImagesReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // run image processing in different thread
        QFuture<void> future = QtConcurrent::run([=](){
            QImage image;

            if (!image.load(reply, nullptr)) {
                qDebug() << "ERROR LOADING IMAGE";
            }

            ////////////////////////////////////////////////////////////////////
            /// GET DOMINANT COLOR
            ////////////////////////////////////////////////////////////////////
            m_pixelColor = dominantColor(image);

            // if the color is too bright, return black instead
            if (m_pixelColor.lightness() > 210) {
                m_pixelColor = QColor("black");
            }
            emit pixelColorChanged();

            ////////////////////////////////////////////////////////////////////
            /// CREATE A SMALL THUMBNAIL IMAGE
            ////////////////////////////////////////////////////////////////////
            QImage smallImage = image;
            smallImage.scaledToHeight(90, Qt::SmoothTransformation);

            // create byte array and then convert to base64
            QByteArray bArray;
            QBuffer buffer(&bArray);
            buffer.open(QIODevice::WriteOnly);
            smallImage.save(&buffer, "JPEG");

            QString bImage("data:image/jpg;base64,");
            bImage.append(QString::fromLatin1(bArray.toBase64().data()));

            m_smallImage = bImage;
            emit smallImageChanged();

            ////////////////////////////////////////////////////////////////////
            /// CREATE LARGE BACKGROUND IMAGE
            ////////////////////////////////////////////////////////////////////
            // resize image
            image.scaledToHeight(800, Qt::SmoothTransformation);

            // create noise layer
            QImage noise(":/images/mini-music-player/noise.png");
            noise = noise.convertToFormat(QImage::Format_ARGB32);

            // create overlay
            QImage overlay(image.width(), image.height(), QImage::Format_ARGB32);
            overlay.fill(Qt::transparent);

            QImage fill(image.width(), image.height(), QImage::Format_ARGB32);
            fill.fill(Qt::black);

            QPainter overlayP(&overlay);
            overlayP.setOpacity(0.6);
            overlayP.drawImage(QRect(0, 0, overlay.width(), overlay.height()), fill);

            // merge the images together
            QPainter painter(&image);
            painter.drawImage(image.rect(), overlay);
            painter.end();
            painter.begin(&image);
            painter.drawImage(image.rect(), noise);

            // create byte array and then convert to base64
            QByteArray lArray;
            QBuffer lBuffer(&lArray);
            lBuffer.open(QIODevice::WriteOnly);
            image.save(&lBuffer, "JPEG");

            QString lImage("data:image/jpg;base64,");
            lImage.append(QString::fromLatin1(lArray.toBase64().data()));

            m_image = lImage;
            emit imageChanged();

            reply->deleteLater();
        });

    } else {
        qDebug() << "ERROR LOADING IMAGE" << reply->errorString();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ADD NOISE TO IMAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MediaPlayerUtils::addNoise(QString url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MediaPlayerUtils::addNoiseReply);
    manager->get(QNetworkRequest(QUrl(url)));
}

void MediaPlayerUtils::addNoiseReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // run image processing in different thread
        QFuture<void> future = QtConcurrent::run([=](){
            QImage image;
            if (!image.load(reply, nullptr)) {
                qDebug() << "ERROR LOADING IMAGE";
            }
            // resize image
            image.scaledToHeight(800, Qt::SmoothTransformation);

            // create noise layer
            QImage noise(":/images/mini-music-player/noise.png");
            noise = noise.convertToFormat(QImage::Format_ARGB32);

            // create overlay
            QImage overlay(image.width(), image.height(), QImage::Format_ARGB32);
            overlay.fill(Qt::transparent);

            QImage fill(image.width(), image.height(), QImage::Format_ARGB32);
            fill.fill(Qt::black);

            QPainter overlayP(&overlay);
            overlayP.setOpacity(0.6);
            overlayP.drawImage(QRect(0, 0, overlay.width(), overlay.height()), fill);

            // merge the images together
            QPainter painter(&image);
            painter.drawImage(image.rect(), overlay);
            painter.end();
            painter.begin(&image);
            painter.drawImage(image.rect(), noise);

            // create byte array and then convert to base64
            QByteArray bArray;
            QBuffer buffer(&bArray);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "JPEG");

            QString bImage("data:image/jpg;base64,");
            bImage.append(QString::fromLatin1(bArray.toBase64().data()));

//            m_miniMuiscPlayerImage = bImage;
//            emit miniMusicPlayerImageChanged();
        });
    } else {
        qDebug() << "ERROR LOADING IMAGE" << reply->errorString();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DOMINANT COLOR OF IMAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QColor MediaPlayerUtils::dominantColor(const QImage &image)
{
    qint32 averageRed = 0, averageGreen = 0, averageBlue = 0;

    const qint32 maxX = qMin<qint32>(image.width(), 0 + image.width());
    const qint32 maxY = qMin<qint32>(image.height(), 0 + image.height());

    for (qint32 y = 0; y < maxY; y++)  {
        for (qint32 x = 0; x < maxX; x++)   {
            QRgb pixel = image.pixel(x, y);

            averageRed += qRed(pixel);
            averageGreen += qGreen(pixel);
            averageBlue += qBlue(pixel);
        }
    }

    qint32 n = image.width() * image.height();

    Q_ASSERT(n);
    if (n <= 0)
        return Qt::black;

    return QColor(averageRed / n, averageGreen / n, averageBlue / n);
}
