#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "entity.h"

class Remote : public Entity
{
    Q_OBJECT

public:
    Q_PROPERTY  (QVariantList   commands       READ    commands       NOTIFY commandsChanged)
    Q_INVOKABLE bool            update(const QVariantMap& attributes) override;

    QVariantList                commands() const { return m_commands; }


    explicit Remote();
    Remote(const QVariantMap& config, QObject* integrationObj);

signals:
    void commandsChanged();

public:
    static QString      Type;

    static QStringList&         AllFeatures()
    {
        static QStringList      s
        {
            // Transport and mediacontrols
            "PLAY", "PAUSE", "PLAYTOGGLE", "STOP", "FORWARD", "REVERSE", "NEXT", "PREVIOUS", "INFO", "RECORDINGS", "RECORD", "LIVE",

            // digits
            "DIGIT 0", "DIGIT 1","DIGIT 2", "DIGIT 3", "DIGIT 4", "DIGIT 5", "DIGIT 6", "DIGIT 7", "DIGIT 8", "DIGIT 9", "DIGIT 10", "DIGIT 10+", "DIGIT 11", "DIGIT 12", "DIGIT_SEPARATOR", "DIGIT_ENTER,"

            // navigation
            "CURSOR_UP", "CURSOR_DOWN", "CURSOR_LEFT", "CURSOR_RIGHT", "CURSOR_OK", "BACK", "HOME", "MENU", "EXIT", "INFO", "APP",

            // power
            "POWER_OFF", "POWER_ON", "POWER_TOGGLE",

            //tuner
            "CHANNEL_UP", "CHANNEL_DOWN", "CHANNEL_SEARCH", "FAVORITE", "GUIDE",

            // interactive
            "FUNCTION_RED", "FUNCTION_GREEN", "FUNCTION_YELLOW", "FUNCTION_BLUE", "FUNCTION_ORANGE",

            // video
            "FORMAT_16_9", "FORMAT_4_3", "FORMAT_AUTO",

            // volume
            "VOLUME_UP", "VOLUME_DOWN", "MUTE_TOGGLE",

            // input
            "INPUT_TUNER_1", "INPUT_TUNER_2", "INPUT_TUNER_X", "INPUT_HDMI_1", "INPUT_HDMI_2", "INPUT_HDMI_X", "INPUT_X_1", "INPUT_X_2",

            // output
            "OUTPUT_HDMI_1", "OUTPUT_HDMI_2", "OUTPUT_DVI_1", "OUTPUT_AUDIO_X", "OUTPUT_X",

            // services
            "SERVICE_NETFLIX", "SERVICE_HULU"

        };

        return s;
    }

private:
    QVariantList        m_commands;

};

#endif // REMOTE_H
