#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>

class Utils : public QObject
{
    Q_OBJECT

public:
    explicit Utils(QObject *parent = nullptr);
    virtual ~Utils();

    Q_PROPERTY  (QColor pixelColor      READ pixelColor         NOTIFY pixelColorChanged)

    QColor                      pixelColor              () { return m_pixelColor; }

    // Image manipulation helpers
    QImage                      resizeImage             (QImage* image);
    QColor                      getPixelColor           (QImage* image, const int &x, const int &y);
    Q_INVOKABLE void            getPixelColor           (QString url);
    QImage                      addNoise                (QImage image);

    static Utils*               getInstance ()
    { return s_instance; }

signals:
    void pixelColorChanged();

private:
    static Utils*               s_instance;
    QImage*                     m_image = nullptr;
    QColor                      m_pixelColor;

private slots:
    void getPixelColorReply(QNetworkReply* reply);

};
#endif // UTILS_H
