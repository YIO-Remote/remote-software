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

class Utils : public QObject
{
    Q_OBJECT

public:
    explicit Utils(QObject *parent = nullptr);
    virtual ~Utils();

    Q_PROPERTY  (QColor     pixelColor                  READ pixelColor                 NOTIFY pixelColorChanged)
    Q_PROPERTY  (QString    miniMusicPlayerImage        READ miniMusicPlayerImage       NOTIFY miniMusicPlayerImageChanged)

    QColor                      pixelColor              () { return m_pixelColor; }
    QString                     miniMusicPlayerImage    () { return m_miniMuiscPlayerImage; }

    // Image manipulation helpers
    QColor                      getPixelColor           (QImage* image, const int &x, const int &y);
    Q_INVOKABLE void            getPixelColor           (QString url);
    QColor                      dominantColor           (const QImage& image);
    Q_INVOKABLE void            addNoise                (QString url);

    static Utils*               getInstance ()
    { return s_instance; }

signals:
    void pixelColorChanged();
    void miniMusicPlayerImageChanged();

private:
    static Utils*               s_instance;
    QString                     m_miniMuiscPlayerImage;
    QColor                      m_pixelColor;

private slots:
    void                        getPixelColorReply     (QNetworkReply* reply);
    void                        addNoiseReply          (QNetworkReply* reply);

};
#endif // UTILS_H
