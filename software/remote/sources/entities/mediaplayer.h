#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "entity.h"

class MediaPlayer : public Entity
{
    Q_OBJECT

public:

    enum states {
        OFF=0,
        ON=1,
        IDLE=2,
        PLAYING=3
    };

    Q_ENUM(states)

    // properties
    Q_PROPERTY  (states         state       READ    state       NOTIFY      stateChanged)
    Q_PROPERTY  (double         volume      READ    volume      NOTIFY      volumeChanged)
    Q_PROPERTY  (bool           muted       READ    muted       NOTIFY      mutedChanged)
    Q_PROPERTY  (QString        mediaType   READ    mediaType   NOTIFY      mediaTypeChanged)
    Q_PROPERTY  (QString        mediaTitle  READ    mediaTitle  NOTIFY      mediaTitleChanged)
    Q_PROPERTY  (QString        mediaArtist READ    mediaArtist NOTIFY      mediaArtistChanged)
    Q_PROPERTY  (QString        mediaImage  READ    mediaImage  NOTIFY      mediaImageChanged)

    double                      volume() { return m_volume; }
    bool                        muted()  { return m_muted; }
    QString                     mediaType() { return m_mediaType; }
    QString                     mediaImage() { return m_mediaImage; }
    QString                     mediaTitle() { return m_mediaTitle; }
    QString                     mediaArtist() { return m_mediaArtist; }

    // methods
    Q_INVOKABLE bool            update(const QVariantMap& attributes) override;
    Q_INVOKABLE void            setVolume(double value);

    // constructor
    explicit MediaPlayer();
    MediaPlayer(const QVariantMap& config, QObject* integrationObj);

    void                        init(const QVariantMap& config);

    enum states                 state() { return m_state; }

signals:
    void stateChanged();
    void playing(QString name);
    void volumeChanged();
    void mutedChanged();
    void mediaTypeChanged();
    void mediaTitleChanged();
    void mediaArtistChanged();
    void mediaImageChanged();

public:
    static QString Type;

    static QStringList&         AllFeatures()
    {
        static QStringList      s
        {
            "SOURCE", "APP_NAME",
            "VOLUME", "VOLUME_UP", "VOLUME_DOWN", "VOLUME_SET","MUTE","MUTE_SET",
            "MEDIA_TYPE", "MEDIA_TITLE", "MEDIA_ARTIST", "MEDIA_ALBUM", "MEDIA_DURATION", "MEDIA_POSITION", "MEDIA_IMAGE",
            "PLAY", "PAUSE", "STOP", "PREVIOUS", "NEXT", "SEEK", "SHUFFLE", "TURN_ON", "TURN_OFF"
        };

        return s;
    }

private:
    states                      m_state;
    double                      m_volume;
    bool                        m_muted;
    QString                     m_mediaType;
    QString                     m_mediaImage;
    QString                     m_mediaTitle;
    QString                     m_mediaArtist;
};

#endif // MEDIAPLAYER_H
