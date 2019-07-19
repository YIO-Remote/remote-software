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

    Q_PROPERTY  (states         state       READ    state       NOTIFY stateChanged)

    Q_INVOKABLE bool            update(const QVariantMap& attributes) override;

    explicit MediaPlayer();
    MediaPlayer(const QVariantMap& config, QObject* integrationObj);

    enum states                 state() const { return m_state; }

    void                        init(const QVariantMap& config);

signals:
    void stateChanged();

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
};

#endif // MEDIAPLAYER_H
