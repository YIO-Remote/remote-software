#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMetaEnum>
#include "entity.h"
#include "mediaplayerinterface.h"

class MediaPlayer : public Entity, MediaPlayerInterface
{
    Q_OBJECT
    Q_INTERFACES(MediaPlayerInterface)
public:
    // I see no other solution for cloning an enum for use in QML
    enum class States {
        OFF         = int (MediaPlayerDef::States::OFF),
        ON          = int (MediaPlayerDef::States::ON),
        IDLE        = int (MediaPlayerDef::States::IDLE),
        PLAYING     = int (MediaPlayerDef::States::PLAYING)
    };
    Q_ENUM(States)
    // properties
    Q_PROPERTY  (States                     state           READ    state           NOTIFY      stateChanged)
    Q_PROPERTY  (int                        volume          READ    volume          NOTIFY      volumeChanged)
    Q_PROPERTY  (bool                       muted           READ    muted           NOTIFY      mutedChanged)
    Q_PROPERTY  (QString                    mediaType       READ    mediaType       NOTIFY      mediaTypeChanged)
    Q_PROPERTY  (QString                    mediaTitle      READ    mediaTitle      NOTIFY      mediaTitleChanged)
    Q_PROPERTY  (QString                    mediaArtist     READ    mediaArtist     NOTIFY      mediaArtistChanged)
    Q_PROPERTY  (QString                    mediaImage      READ    mediaImage      NOTIFY      mediaImageChanged)
    Q_PROPERTY  (QString                    source          READ    source          NOTIFY      sourceChanged)
    Q_PROPERTY  (QVariant                   browseResult    READ    browseResult    NOTIFY      browseResultChanged)

    States                      state()                 { return m_state; }
    MediaPlayerDef::States      getState() override     { return static_cast<MediaPlayerDef::States>(m_state); }
    int                         volume() override       { return m_volume; }
    bool                        muted() override        { return m_muted; }
    QString                     mediaType() override    { return m_mediaType; }
    QString                     mediaImage() override   { return m_mediaImage; }
    QString                     mediaTitle() override   { return m_mediaTitle; }
    QString                     mediaArtist() override  { return m_mediaArtist; }
    QString                     source() override       { return m_source; }

    // extension for "generic" media browsing
    QVariant                    browseResult() override { return m_browseResult; }

    // update an entity's attributes
    Q_INVOKABLE bool            update              (const QVariantMap& attributes) override;
    Q_INVOKABLE bool            updateAttrByName    (const QString& name, const QVariant& value) override;
    Q_INVOKABLE bool            updateAttrByIndex   (int attrIndex, const QVariant& value) override;

    // attribute name and index
    Q_INVOKABLE QString         getAttrName         (int attrIndex) override;
    Q_INVOKABLE int             getAttrIndex        (const QString& attrName) override;

    Q_INVOKABLE void            turnOn();
    Q_INVOKABLE void            turnOff();
    Q_INVOKABLE void            play();
    Q_INVOKABLE void            pause();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            previous();
    Q_INVOKABLE void            next();
    Q_INVOKABLE void            setVolume(double value);
    Q_INVOKABLE void            volumeUp();
    Q_INVOKABLE void            volumeDown();

    // extension for "generic" media browsing
    Q_INVOKABLE void            browse                  (QString command);        // Command item_key, "TOP", "BACK", "PLAY"
    Q_INVOKABLE void            playMedia               (const QString& command, const QString& itemKey); // command PLAY, QUEUE
    Q_INVOKABLE void            search                  (const QString& searchText, const QString& itemKey); // Search

    // only for C++ integrations
    virtual     void*           getSpecificInterface    () override;

    // constructor
    MediaPlayer(const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

signals:
    void stateChanged();
    void volumeChanged();
    void mutedChanged();
    void mediaTypeChanged();
    void mediaTitleChanged();
    void mediaArtistChanged();
    void mediaImageChanged();
    void sourceChanged();
    void browseResultChanged();

public:
    static QString Type;

    static QStringList&         AllFeatures()
    {
        static QStringList s;
        if (s.count() == 0)
            s.append(allFeatures());
        return s;
    }
    static QStringList&         AllCommands()
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
    static QStringList                  allFeatures     ();
    static QMetaEnum                    s_metaEnum;

    States                              m_state;
    int                                 m_volume;
    bool                                m_muted;
    QString                             m_mediaType;
    QString                             m_mediaImage;
    QString                             m_mediaTitle;
    QString                             m_mediaArtist;
    QString                             m_source;

    // extension for "generic" media browsing
    QVariant                    m_browseResult;         // Better to return in one structure (perf, synchronisation) :
                                                        // items :          Array of { item_key, title, sub_title, image_url, input_prompt for search }
                                                        // playCommands:    Array of play commands for the items like PLAY, QUEUE
                                                        // type:            Item type Genre, Album, Artist, Track, Playlist, Radio
                                                        // title:           Name of the genre, album, artist, track
                                                        // level:           Top is 0
};

#endif // MEDIAPLAYER_H
