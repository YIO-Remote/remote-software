#ifndef MEDIAPLAYERINTERFACE_H
#define MEDIAPLAYERINTERFACE_H

#include <QObject>

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class MediaPlayerDef : public QObject
{
    Q_OBJECT
public:
    enum Attributes  {  STATE, SOURCE, VOLUME, MUTED, MEDIATYPE, MEDIATITLE, MEDIAARTIST, MEDIAIMAGE, BROWSERESULT };
    Q_ENUM (Attributes)

    enum States   { OFF=0, ON=1, IDLE=2, PLAYING=3 };
    Q_ENUM (States)

    explicit MediaPlayerDef (QObject *parent = nullptr) : QObject(parent)
    {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class MediaPlayerInterface
{
public:
    virtual ~MediaPlayerInterface  ();

    virtual int                         volume() = 0;
    virtual bool                        muted() = 0;
    virtual QString                     mediaType() = 0;
    virtual QString                     mediaImage() = 0;
    virtual QString                     mediaTitle() = 0;
    virtual QString                     mediaArtist() = 0;
    virtual QString                     source() = 0;

    // extension for "generic" media browsing
    virtual QVariant                    browseResult() = 0;
};

QT_BEGIN_NAMESPACE
#define MediaPlayerInterface_iid "YIO.MediaPlayerInterface"
Q_DECLARE_INTERFACE(MediaPlayerInterface, MediaPlayerInterface_iid)
QT_END_NAMESPACE

#endif // MEDIAPLAYERINTERFACE_H
