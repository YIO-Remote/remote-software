#include "utils.h"

#include <QDebug>

Utils* Utils::s_instance = NULL;

Utils::Utils(QObject *parent) : QObject(parent)
{
    s_instance = this;

}

Utils::~Utils()
{
    s_instance = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GET PIXEL COLOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QColor Utils::getPixelColor(QImage *image, const int &x, const int &y)
{
    QColor pixelColor( image->pixel(x, y) );

    return pixelColor;
}

void Utils::getPixelColor(QString url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Utils::getPixelColorReply);
    manager->get(QNetworkRequest(QUrl(url)));
}

void Utils::getPixelColorReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QImage image;
        if (!image.load(reply, nullptr)) {
            qDebug() << "ERROR LOADING IMAGE";
        }

        m_pixelColor = dominantColor(image);

        // if the color is too bright, return black instead
        if (m_pixelColor.lightness() > 210) {
            m_pixelColor = QColor("black");
        }
        emit pixelColorChanged();

        reply->deleteLater();

    } else {
        qDebug() << "ERROR LOADING IMAGE" << reply->errorString();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ADD NOISE TO IMAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Utils::addNoise(QString url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Utils::addNoiseReply);
    manager->get(QNetworkRequest(QUrl(url)));
}

void Utils::addNoiseReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QImage image;
        if (!image.load(reply, nullptr)) {
            qDebug() << "ERROR LOADING IMAGE";
        }

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

        m_miniMuiscPlayerImage = bImage;
        emit miniMusicPlayerImageChanged();

    } else {
        qDebug() << "ERROR LOADING IMAGE" << reply->errorString();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DOMINANT COLOR OF IMAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QColor Utils::dominantColor(const QImage &image)
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
