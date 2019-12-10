#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "entity.h"
#include "remoteinterface.h"

class Remote : public Entity, RemoteInterface
{
    Q_OBJECT
    Q_INTERFACES(RemoteInterface)
public:
    Q_PROPERTY  (QVariantList   commands       READ    commands       NOTIFY commandsChanged)

    QVariantList                commands() override { return m_commands; }

    // transport and media controls
    Q_INVOKABLE void            play();
    Q_INVOKABLE void            pause();
    Q_INVOKABLE void            playToggle();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            forward();
    Q_INVOKABLE void            backward();
    Q_INVOKABLE void            next();
    Q_INVOKABLE void            previous();
    Q_INVOKABLE void            info();
    Q_INVOKABLE void            recordings();
    Q_INVOKABLE void            record();
    Q_INVOKABLE void            live();

    // navigation
    Q_INVOKABLE void            cursorUp();
    Q_INVOKABLE void            cursorDown();
    Q_INVOKABLE void            cursorLeft();
    Q_INVOKABLE void            cursorRight();
    Q_INVOKABLE void            cursorOK();
    Q_INVOKABLE void            back();
    Q_INVOKABLE void            home();
    Q_INVOKABLE void            menu();
    Q_INVOKABLE void            exit();
    Q_INVOKABLE void            app();

    // power commands
    Q_INVOKABLE void            powerOn();
    Q_INVOKABLE void            powerOff();
    Q_INVOKABLE void            powerToggle();

    // tuner commands
    Q_INVOKABLE void            channelUp();
    Q_INVOKABLE void            channelDown();
    Q_INVOKABLE void            channelSearch();
    Q_INVOKABLE void            toFavorite();
    Q_INVOKABLE void            guide();

    // volume commands
    Q_INVOKABLE void            volumeUp();
    Q_INVOKABLE void            volumeDown();
    Q_INVOKABLE void            muteToggle();

    void                        turnOn() override               { powerOn (); }
    void                        turnOff() override              { powerOff (); }


    explicit Remote             (QObject *parent = nullptr);
    Remote                      (const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent = nullptr);

    static void staticInitialize();

signals:
    void commandsChanged();

public:
    static QString      Type;

private:
    static QMetaEnum    s_metaEnumCommands;
    static QMetaEnum    s_metaEnumAttr;
    static QMetaEnum    s_metaEnumFeatures;
    static QMetaEnum    s_metaEnumState;

    QVariantList        m_commands;

};

#endif // REMOTE_H
