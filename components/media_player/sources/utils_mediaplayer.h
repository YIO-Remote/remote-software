#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QBuffer>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

class MediaPlayerUtils : public QObject
{
    Q_OBJECT

public:
    explicit MediaPlayerUtils() {}
    virtual ~MediaPlayerUtils() {}

    Q_PROPERTY  (QString    imageURL                    READ imageURL                   WRITE setImageURL)

    QString imageURL()
    {
        return m_imageURL;
    }

    void setImageURL(QString url)
    {
        m_imageURL = url;
        generateImages(url);
    }

    Q_PROPERTY  (QString    image                       READ image                      NOTIFY imageChanged)

    QString image()
    {
        return m_image;
    }

    Q_PROPERTY  (QString    smallImage                  READ smallImage                 NOTIFY smallImageChanged)

    QString smallImage()
    {
        return m_smallImage;
    }

    Q_PROPERTY  (QColor     pixelColor                  READ pixelColor                 NOTIFY pixelColorChanged)

    QColor                      pixelColor              () { return m_pixelColor; }


signals:
    void imageChanged();
    void smallImageChanged();
    void pixelColorChanged();

private:
    QString                     m_imageURL;
    QString                     m_image;
    QString                     m_smallImage;
    QColor                      m_pixelColor;

    void                        generateImages          (QString url);
    QColor                      dominantColor           (const QImage& image);


private slots:
    void                        generateImagesReply    (QNetworkReply* reply);

};
#endif // UTILS_H
