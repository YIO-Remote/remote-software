#pragma once

#include <QObject>

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class RemoteDef : public QObject {
    Q_OBJECT
 public:
    enum Attributes { STATE };
    Q_ENUM(Attributes)

    enum States { OFFLINE = 0, ONLINE = 1 };
    Q_ENUM(States)

    enum Features {
        // Transport and mediacontrols
        F_PLAY,
        F_PAUSE,
        F_PLAYTOGGLE,
        F_STOP,
        F_FORWARD,
        F_BACKWARD,
        F_NEXT,
        F_PREVIOUS,
        F_INFO,
        F_RECORDINGS,
        F_RECORD,
        F_LIVE,

        // digits
        F_DIGIT_0,
        F_DIGIT_1,
        F_DIGIT_2,
        F_DIGIT_3,
        F_DIGIT_4,
        F_DIGIT_5,
        F_DIGIT_6,
        F_DIGIT_7,
        F_DIGIT_8,
        F_DIGIT_9,
        F_DIGIT_10,
        F_DIGIT_10plus,
        F_DIGIT_11,
        F_DIGIT_12,
        F_DIGIT_SEPARATOR,
        F_DIGIT_ENTER,

        // navigation
        F_CURSOR_UP,
        F_CURSOR_DOWN,
        F_CURSOR_LEFT,
        F_CURSOR_RIGHT,
        F_CURSOR_OK,
        F_BACK,
        F_HOME,
        F_MENU,
        F_EXIT,
        F_APP,  // INFO is repeated twice

        // power
        F_POWER_OFF,
        F_POWER_ON,
        F_POWER_TOGGLE,

        // tuner
        F_CHANNEL_UP,
        F_CHANNEL_DOWN,
        F_CHANNEL_SEARCH,
        F_FAVORITE,
        F_GUIDE,

        // interactive
        F_FUNCTION_RED,
        F_FUNCTION_GREEN,
        F_FUNCTION_YELLOW,
        F_FUNCTION_BLUE,
        F_FUNCTION_ORANGE,

        // video
        F_FORMAT_16_9,
        F_FORMAT_4_3,
        F_FORMAT_AUTO,

        // volume
        F_VOLUME_UP,
        F_VOLUME_DOWN,
        F_MUTE_TOGGLE,

        // input
        F_SOURCE,
        F_INPUT_TUNER_1,
        F_INPUT_TUNER_2,
        F_INPUT_TUNER_X,
        F_INPUT_HDMI_1,
        F_INPUT_HDMI_2,
        F_INPUT_HDMI_X,
        F_INPUT_X_1,
        F_INPUT_X_2,

        // output
        F_OUTPUT_HDMI_1,
        F_OUTPUT_HDMI_2,
        F_OUTPUT_DVI_1,
        F_OUTPUT_AUDIO_X,
        F_OUTPUT_X,

        // services
        F_SERVICE_NETFLIX,
        F_SERVICE_HULUF_
    };
    Q_ENUM(Features)

    enum Commands {
        // Not entity related but mus be defined somewhere
        C_REMOTE_CHARGED,
        C_REMOTE_LOWBATTERY,

        // Transport and mediacontrols
        C_PLAY,
        C_PAUSE,
        C_PLAYTOGGLE,
        C_STOP,
        C_FORWARD,
        C_BACKWARD,
        C_NEXT,
        C_PREVIOUS,
        C_INFO,
        C_RECORDINGS,
        C_RECORD,
        C_LIVE,

        // digits
        C_DIGIT_0,
        C_DIGIT_1,
        C_DIGIT_2,
        C_DIGIT_3,
        C_DIGIT_4,
        C_DIGIT_5,
        C_DIGIT_6,
        C_DIGIT_7,
        C_DIGIT_8,
        C_DIGIT_9,
        C_DIGIT_10,
        C_DIGIT_10plus,
        C_DIGIT_11,
        C_DIGIT_12,
        C_DIGIT_SEPARATOR,
        C_DIGIT_ENTER,

        // navigation
        C_CURSOR_UP,
        C_CURSOR_DOWN,
        C_CURSOR_LEFT,
        C_CURSOR_RIGHT,
        C_CURSOR_OK,
        C_BACK,
        C_HOME,
        C_MENU,
        C_EXIT,
        C_APP,  // INFO is repeated twice

        // power
        C_POWER_OFF,
        C_POWER_ON,
        C_POWER_TOGGLE,

        // tuner
        C_CHANNEL_UP,
        C_CHANNEL_DOWN,
        C_CHANNEL_SEARCH,
        C_FAVORITE,
        C_GUIDE,

        // interactive
        C_FUNCTION_RED,
        C_FUNCTION_GREEN,
        C_FUNCTION_YELLOW,
        C_FUNCTION_BLUE,
        C_FUNCTION_ORANGE,

        // video
        C_FORMAT_16_9,
        C_FORMAT_4_3,
        C_FORMAT_AUTO,

        // volume
        C_VOLUME_UP,
        C_VOLUME_DOWN,
        C_MUTE_TOGGLE,

        // input
        C_SOURCE,
        C_INPUT_TUNER_1,
        C_INPUT_TUNER_2,
        C_INPUT_TUNER_X,
        C_INPUT_HDMI_1,
        C_INPUT_HDMI_2,
        C_INPUT_HDMI_X,
        C_INPUT_X_1,
        C_INPUT_X_2,

        // output
        C_OUTPUT_HDMI_1,
        C_OUTPUT_HDMI_2,
        C_OUTPUT_DVI_1,
        C_OUTPUT_AUDIO_X,
        C_OUTPUT_X,

        // services
        C_SERVICE_NETFLIX,
        C_SERVICE_HULUC_
    };
    Q_ENUM(Commands)

    explicit RemoteDef(QObject *parent = nullptr) : QObject(parent) {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class RemoteInterface {
 public:
    virtual ~RemoteInterface();
    virtual QVariantList commands() = 0;
    virtual QVariantList channels() = 0;
    virtual QVariantMap  settings() = 0;
};

QT_BEGIN_NAMESPACE
#define RemoteInterface_iid "YIO.RemoteInterface"
Q_DECLARE_INTERFACE(RemoteInterface, RemoteInterface_iid)
QT_END_NAMESPACE
