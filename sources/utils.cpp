#include "utils.h"

#include <QDebug>

Utils* Utils::s_instance = NULL;

Utils::Utils(QObject *parent) : QObject(parent)
{
    s_instance = this;

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &Utils::netWorkReply);
}

Utils::~Utils()
{
    s_instance = NULL;
}

QImage Utils::resizeImage(QImage *image)
{
    QImage rImage = image->scaled(800, 800, Qt::KeepAspectRatio);
    return rImage;
}

QColor Utils::getPixelColor(QImage *image, const int &x, const int &y)
{
    QColor pixelColor( image->pixel(x, y) );

    return pixelColor;
}

void Utils::getPixelColor(QString url)
{
    m_manager->get(QNetworkRequest(QUrl(url)));
}

QImage Utils::addNoise(QImage image)
{
    QImage rImage;
    QImage noise(":/images/mini-music-player/noise.png");

    QPainter painter(&rImage);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.drawImage(rImage.rect(),image);
    painter.drawImage(rImage.rect(), noise);

    return rImage;
}

void Utils::netWorkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QImage image;
        if (!image.load(reply, nullptr)) {
            qDebug() << "ERROR LOADING IMAGE";
        }

        image.scaled(1, 1, Qt::IgnoreAspectRatio);
        m_pixelColor = image.pixelColor(0, 0);

        if (m_pixelColor.lightness() > 200) {
            m_pixelColor = QColor("black");
        }
        emit pixelColorChanged();

        reply->deleteLater();

    } else {
        qDebug() << "ERROR LOADING IMAGE" << reply->errorString();
    }
}
