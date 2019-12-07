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
    // properties
    Q_PROPERTY  (int                        volume          READ    volume          NOTIFY      volumeChanged)
    Q_PROPERTY  (bool                       muted           READ    muted           NOTIFY      mutedChanged)
    Q_PROPERTY  (QString                    mediaType       READ    mediaType       NOTIFY      mediaTypeChanged)
    Q_PROPERTY  (QString                    mediaTitle      READ    mediaTitle      NOTIFY      mediaTitleChanged)
    Q_PROPERTY  (QString                    mediaArtist     READ    mediaArtist     NOTIFY      mediaArtistChanged)
    Q_PROPERTY  (QString                    mediaImage      READ    mediaImage      NOTIFY      mediaImageChanged)
    Q_PROPERTY  (QString                    source          READ    source          NOTIFY      sourceChanged)
    Q_PROPERTY  (QVariant                   browseResult    READ    browseResult    NOTIFY      browseResultChanged)
    Q_PROPERTY  (QObject*                   searchModel     READ    searchModel     NOTIFY      searchModelChanged)
    Q_PROPERTY  (QObject*                   browseModel     READ    browseModel     NOTIFY      browseModelChanged)

    int                         volume() override       { return m_volume; }
    bool                        muted() override        { return m_muted; }
    QString                     mediaType() override    { return m_mediaType; }
    QString                     mediaImage() override   { return m_mediaImage; }
    QString                     mediaTitle() override   { return m_mediaTitle; }
    QString                     mediaArtist() override  { return m_mediaArtist; }
    QString                     source() override       { return m_source; }

    // extension for "generic" media browsing
    QVariant                    browseResult() override { return m_browseResult; }
    QObject*                    searchModel() { return m_searchModel; }
    QObject*                    browseModel() { return m_browseModel; }

    // update an entity's attributes
    Q_INVOKABLE void            play();
    Q_INVOKABLE void            pause();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            previous();
    Q_INVOKABLE void            next();
    Q_INVOKABLE void            setVolume(int value);
    Q_INVOKABLE void            volumeUp();
    Q_INVOKABLE void            volumeDown();

    // extension for "generic" media browsing
    Q_INVOKABLE void            browse                  (QString command);        // Command item_key, "TOP", "BACK", "PLAY"
    Q_INVOKABLE void            playMedia               (const QString& itemKey);
    Q_INVOKABLE void            search                  (const QString& searchText, const QString& itemKey); // Search
    Q_INVOKABLE void            search                  (const QString& searchText);

    void                        setSearchModel(QObject* model) override;
    void                        setBrowseModel(QObject* model) override;

    bool                        isOn() override         { return m_state == MediaPlayerDef::ON || m_state == MediaPlayerDef::PLAYING; }
    bool                        updateAttrByIndex       (int attrIndex, const QVariant& value) override;
    void                        turnOn                  () override;
    void                        turnOff                 () override;

    // constructor
    MediaPlayer(const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

signals:
    void volumeChanged();
    void mutedChanged();
    void mediaTypeChanged();
    void mediaTitleChanged();
    void mediaArtistChanged();
    void mediaImageChanged();
    void sourceChanged();
    void browseResultChanged();
    void searchModelChanged();
    void browseModelChanged();

public:
    static QString Type;
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
    int                         m_volume;
    bool                        m_muted;
    QString                     m_mediaType;
    QString                     m_mediaImage;
    QString                     m_mediaTitle;
    QString                     m_mediaArtist;
    QString                     m_source;

    QObject*                    m_searchModel = nullptr;
    /*                         {
     *                         albums:          array of {id, title, subtitle, image_url, array of commands}
     *                         tracks:          array of {id, title, subtitle, image_url, array of commands}
     *                         artists:         array of {id, title, subtitle, image_url, array of commands}
     *                         playlists:       array of {id, title, subtitle, image_url, array of commands}
     *                         }
    */
    QObject*                    m_browseModel = nullptr;


    // extension for "generic" media browsing
    QVariant                    m_browseResult;         // Better to return in one structure (perf, synchronisation) :
    // items :          Array of { item_key, title, sub_title, image_url, input_prompt for search }
    // playCommands:    Array of play commands for the items like PLAY, QUEUE
    // type:            Item type Genre, Album, Artist, Track, Playlist, Radio
    // title:           Name of the genre, album, artist, track
    // level:           Top is 0
};

#endif // MEDIAPLAYER_H
