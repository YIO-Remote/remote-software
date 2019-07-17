#include <QtQml/qqmlprivate.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>

static const unsigned char qt_resource_tree[] = {
0,
0,0,0,0,2,0,0,0,8,0,0,0,1,0,0,0,
8,0,0,0,0,0,1,0,0,0,0,0,0,0,48,0,
2,0,0,0,3,0,0,0,49,0,0,1,16,0,2,0,
0,0,2,0,0,0,39,0,0,0,250,0,0,0,0,0,
1,0,0,0,0,0,0,2,146,0,2,0,0,0,15,0,
0,0,11,0,0,1,236,0,2,0,0,0,2,0,0,0,
9,0,0,2,104,0,0,0,0,0,1,0,0,0,0,0,
0,2,64,0,0,0,0,0,1,0,0,0,0,0,0,2,
24,0,0,0,0,0,1,0,0,0,0,0,0,2,0,0,
0,0,0,0,1,0,0,0,0,0,0,5,216,0,2,0,
0,0,4,0,0,0,35,0,0,3,208,0,0,0,0,0,
1,0,0,0,0,0,0,3,160,0,0,0,0,0,1,0,
0,0,0,0,0,4,138,0,0,0,0,0,1,0,0,0,
0,0,0,3,80,0,0,0,0,0,1,0,0,0,0,0,
0,3,30,0,0,0,0,0,1,0,0,0,0,0,0,2,
246,0,0,0,0,0,1,0,0,0,0,0,0,2,206,0,
0,0,0,0,1,0,0,0,0,0,0,3,122,0,0,0,
0,0,1,0,0,0,0,0,0,4,22,0,0,0,0,0,
1,0,0,0,0,0,0,4,180,0,2,0,0,0,9,0,
0,0,26,0,0,4,66,0,0,0,0,0,1,0,0,0,
0,0,0,2,168,0,0,0,0,0,1,0,0,0,0,0,
0,3,246,0,0,0,0,0,1,0,0,0,0,0,0,4,
108,0,0,0,0,0,1,0,0,0,0,0,0,5,80,0,
0,0,0,0,1,0,0,0,0,0,0,5,108,0,0,0,
0,0,1,0,0,0,0,0,0,4,202,0,0,0,0,0,
1,0,0,0,0,0,0,5,136,0,0,0,0,0,1,0,
0,0,0,0,0,4,238,0,0,0,0,0,1,0,0,0,
0,0,0,5,168,0,0,0,0,0,1,0,0,0,0,0,
0,5,194,0,0,0,0,0,1,0,0,0,0,0,0,5,
52,0,0,0,0,0,1,0,0,0,0,0,0,5,10,0,
0,0,0,0,1,0,0,0,0,0,0,6,64,0,0,0,
0,0,1,0,0,0,0,0,0,6,2,0,0,0,0,0,
1,0,0,0,0,0,0,6,32,0,0,0,0,0,1,0,
0,0,0,0,0,5,232,0,0,0,0,0,1,0,0,0,
0,0,0,1,42,0,2,0,0,0,1,0,0,0,46,0,
0,1,116,0,2,0,0,0,1,0,0,0,41,0,0,1,
58,0,2,0,0,0,4,0,0,0,42,0,0,1,132,0,
0,0,0,0,1,0,0,0,0,0,0,1,198,0,0,0,
0,0,1,0,0,0,0,0,0,1,68,0,0,0,0,0,
1,0,0,0,0,0,0,1,164,0,0,0,0,0,1,0,
0,0,0,0,0,1,58,0,2,0,0,0,2,0,0,0,
47,0,0,1,94,0,0,0,0,0,1,0,0,0,0,0,
0,1,68,0,0,0,0,0,1,0,0,0,0,0,0,0,
78,0,2,0,0,0,1,0,0,0,54,0,0,0,154,0,
2,0,0,0,2,0,0,0,52,0,0,0,118,0,0,0,
0,0,1,0,0,0,0,0,0,0,186,0,0,0,0,0,
1,0,0,0,0,0,0,0,226,0,0,0,0,0,1,0,
0,0,0,0,0,0,94,0,0,0,0,0,1,0,0,0,
0};
static const unsigned char qt_resource_names[] = {
0,
1,0,0,0,47,0,47,0,17,2,192,27,156,0,77,0,
97,0,105,0,110,0,67,0,111,0,110,0,116,0,97,0,
105,0,110,0,101,0,114,0,46,0,113,0,109,0,108,0,
12,6,129,213,147,0,105,0,110,0,116,0,101,0,103,0,
114,0,97,0,116,0,105,0,111,0,110,0,115,0,5,0,
111,99,201,0,104,0,111,0,109,0,101,0,121,0,9,3,
204,138,252,0,104,0,111,0,109,0,101,0,121,0,46,0,
113,0,109,0,108,0,15,13,93,136,28,0,105,0,110,0,
116,0,101,0,103,0,114,0,97,0,116,0,105,0,111,0,
110,0,46,0,113,0,109,0,108,0,13,8,205,216,180,0,
104,0,111,0,109,0,101,0,97,0,115,0,115,0,105,0,
115,0,116,0,97,0,110,0,116,0,17,8,166,207,156,0,
104,0,111,0,109,0,101,0,97,0,115,0,115,0,105,0,
115,0,116,0,97,0,110,0,116,0,46,0,113,0,109,0,
108,0,9,13,247,245,220,0,76,0,105,0,103,0,104,0,
116,0,46,0,113,0,109,0,108,0,8,8,1,90,92,0,
109,0,97,0,105,0,110,0,46,0,113,0,109,0,108,0,
10,7,106,9,51,0,99,0,111,0,109,0,112,0,111,0,
110,0,101,0,110,0,116,0,115,0,5,0,105,48,68,0,
98,0,108,0,105,0,110,0,100,0,2,0,0,7,185,0,
117,0,105,0,10,11,104,113,92,0,66,0,117,0,116,0,
116,0,111,0,110,0,46,0,113,0,109,0,108,0,8,8,
135,97,28,0,67,0,97,0,114,0,100,0,46,0,113,0,
109,0,108,0,5,0,114,253,244,0,108,0,105,0,103,0,
104,0,116,0,13,0,116,69,220,0,67,0,97,0,114,0,
100,0,67,0,111,0,108,0,111,0,114,0,46,0,113,0,
109,0,108,0,14,11,136,26,92,0,67,0,97,0,114,0,
100,0,83,0,119,0,105,0,116,0,99,0,104,0,46,0,
113,0,109,0,108,0,16,8,118,52,252,0,67,0,97,0,
114,0,100,0,68,0,105,0,109,0,109,0,97,0,98,0,
108,0,101,0,46,0,113,0,109,0,108,0,7,9,169,7,
83,0,115,0,99,0,114,0,105,0,112,0,116,0,115,0,
9,3,108,136,147,0,104,0,101,0,108,0,112,0,101,0,
114,0,46,0,106,0,115,0,17,0,114,171,51,0,115,0,
111,0,102,0,116,0,119,0,97,0,114,0,101,0,117,0,
112,0,100,0,97,0,116,0,101,0,46,0,106,0,115,0,
17,15,185,115,156,0,66,0,117,0,116,0,116,0,111,0,
110,0,72,0,97,0,110,0,100,0,108,0,101,0,114,0,
46,0,113,0,109,0,108,0,18,12,65,145,220,0,83,0,
116,0,97,0,110,0,100,0,98,0,121,0,67,0,111,0,
110,0,116,0,114,0,111,0,108,0,46,0,113,0,109,0,
108,0,8,8,159,155,185,0,98,0,97,0,115,0,105,0,
99,0,95,0,117,0,105,0,16,13,176,223,252,0,67,0,
117,0,115,0,116,0,111,0,109,0,66,0,117,0,116,0,
116,0,111,0,110,0,46,0,113,0,109,0,108,0,17,10,
149,63,252,0,76,0,111,0,97,0,100,0,105,0,110,0,
103,0,83,0,99,0,114,0,101,0,101,0,110,0,46,0,
113,0,109,0,108,0,17,10,48,187,188,0,67,0,108,0,
111,0,115,0,105,0,110,0,103,0,83,0,99,0,114,0,
101,0,101,0,110,0,46,0,113,0,109,0,108,0,22,10,
20,242,252,0,78,0,111,0,116,0,105,0,102,0,105,0,
99,0,97,0,116,0,105,0,111,0,110,0,68,0,114,0,
97,0,119,0,101,0,114,0,46,0,113,0,109,0,108,0,
18,9,98,66,92,0,67,0,104,0,97,0,114,0,103,0,
105,0,110,0,103,0,83,0,99,0,114,0,101,0,101,0,
110,0,46,0,113,0,109,0,108,0,16,12,76,186,156,0,
67,0,117,0,115,0,116,0,111,0,109,0,83,0,119,0,
105,0,116,0,99,0,104,0,46,0,113,0,109,0,108,0,
21,5,18,20,28,0,67,0,117,0,115,0,116,0,111,0,
109,0,66,0,117,0,116,0,116,0,111,0,110,0,83,0,
116,0,97,0,116,0,101,0,46,0,113,0,109,0,108,0,
16,3,235,63,220,0,78,0,111,0,116,0,105,0,102,0,
105,0,99,0,97,0,116,0,105,0,111,0,110,0,46,0,
113,0,109,0,108,0,13,13,212,49,188,0,83,0,116,0,
97,0,116,0,117,0,115,0,66,0,97,0,114,0,46,0,
113,0,109,0,108,0,19,12,118,89,28,0,80,0,111,0,
112,0,117,0,112,0,76,0,111,0,119,0,66,0,97,0,
116,0,116,0,101,0,114,0,121,0,46,0,113,0,109,0,
108,0,18,13,94,236,124,0,77,0,97,0,105,0,110,0,
78,0,97,0,118,0,105,0,103,0,97,0,116,0,105,0,
111,0,110,0,46,0,113,0,109,0,108,0,12,15,223,34,
60,0,83,0,101,0,116,0,116,0,105,0,110,0,103,0,
115,0,46,0,113,0,109,0,108,0,18,6,142,239,60,0,
80,0,114,0,111,0,103,0,114,0,101,0,115,0,115,0,
67,0,105,0,114,0,99,0,108,0,101,0,46,0,113,0,
109,0,108,0,8,12,187,11,195,0,115,0,101,0,116,0,
116,0,105,0,110,0,103,0,115,0,15,7,200,60,28,0,
80,0,111,0,119,0,101,0,114,0,83,0,97,0,118,0,
105,0,110,0,103,0,46,0,113,0,109,0,108,0,11,9,
83,209,220,0,68,0,101,0,118,0,105,0,99,0,101,0,
115,0,46,0,113,0,109,0,108,0,18,15,42,182,60,0,
83,0,111,0,102,0,116,0,119,0,97,0,114,0,101,0,
117,0,112,0,100,0,97,0,116,0,101,0,46,0,113,0,
109,0,108,0,11,12,13,67,124,0,66,0,97,0,116,0,
116,0,101,0,114,0,121,0,46,0,113,0,109,0,108,0,
11,2,30,68,220,0,68,0,105,0,115,0,112,0,108,0,
97,0,121,0,46,0,113,0,109,0,108,0,11,6,39,195,
252,0,78,0,101,0,116,0,119,0,111,0,114,0,107,0,
46,0,113,0,109,0,108,0,13,8,207,131,156,0,76,0,
97,0,110,0,103,0,117,0,97,0,103,0,101,0,115,0,
46,0,113,0,109,0,108,0,10,10,183,57,124,0,83,0,
121,0,115,0,116,0,101,0,109,0,46,0,113,0,109,0,
108,0,8,11,150,97,220,0,72,0,117,0,98,0,115,0,
46,0,113,0,109,0,108,0,5,0,118,125,195,0,112,0,
97,0,103,0,101,0,115,0,10,15,157,17,188,0,100,0,
101,0,118,0,105,0,99,0,101,0,46,0,113,0,109,0,
108,0,12,11,223,34,60,0,115,0,101,0,116,0,116,0,
105,0,110,0,103,0,115,0,46,0,113,0,109,0,108,0,
13,13,148,135,60,0,100,0,97,0,115,0,104,0,98,0,
111,0,97,0,114,0,100,0,46,0,113,0,109,0,108,0,
8,8,180,93,60,0,97,0,114,0,101,0,97,0,46,0,
113,0,109,0,108};
static const unsigned char qt_resource_empty_payout[] = { 0, 0, 0, 0, 0 };
QT_BEGIN_NAMESPACE
extern Q_CORE_EXPORT bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);
QT_END_NAMESPACE
namespace QmlCacheGeneratedCode {
namespace _basic_ui_pages_area_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Hubs_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_ProgressCircle_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_System_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_Settings_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Languages_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace __StandbyControl_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_MainNavigation_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace __ButtonHandler_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_pages_dashboard_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _scripts_softwareupdate_js { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Network_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _components_light_ui_CardDimmable_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Display_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _components_light_ui_CardSwitch_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_PopupLowBattery_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_StatusBar_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _components_blind_ui_Card_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _components_light_ui_Button_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace __main_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_Notification_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _components_blind_ui_Button_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _components_light_ui_CardColor_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _integrations_homeassistant_Light_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _integrations_homeassistant_homeassistant_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_CustomButtonState_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_pages_settings_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Battery_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_CustomSwitch_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _integrations_integration_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace __MainContainer_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_ChargingScreen_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Softwareupdate_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_NotificationDrawer_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_ClosingScreen_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_LoadingScreen_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_Devices_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _scripts_helper_js { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_CustomButton_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_settings_PowerSaving_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _integrations_homey_homey_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}
namespace _basic_ui_pages_device_qml { 
    extern const unsigned char qmlData[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), nullptr, nullptr
    };
}

}
namespace {
struct Registry {
    Registry();
    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;
    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, unitRegistry);


Registry::Registry() {
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/pages/area.qml"), &QmlCacheGeneratedCode::_basic_ui_pages_area_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Hubs.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Hubs_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/ProgressCircle.qml"), &QmlCacheGeneratedCode::_basic_ui_ProgressCircle_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/System.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_System_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/Settings.qml"), &QmlCacheGeneratedCode::_basic_ui_Settings_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Languages.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Languages_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/StandbyControl.qml"), &QmlCacheGeneratedCode::__StandbyControl_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/MainNavigation.qml"), &QmlCacheGeneratedCode::_basic_ui_MainNavigation_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/ButtonHandler.qml"), &QmlCacheGeneratedCode::__ButtonHandler_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/pages/dashboard.qml"), &QmlCacheGeneratedCode::_basic_ui_pages_dashboard_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/scripts/softwareupdate.js"), &QmlCacheGeneratedCode::_scripts_softwareupdate_js::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Network.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Network_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/components/light/ui/CardDimmable.qml"), &QmlCacheGeneratedCode::_components_light_ui_CardDimmable_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Display.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Display_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/components/light/ui/CardSwitch.qml"), &QmlCacheGeneratedCode::_components_light_ui_CardSwitch_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/PopupLowBattery.qml"), &QmlCacheGeneratedCode::_basic_ui_PopupLowBattery_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/StatusBar.qml"), &QmlCacheGeneratedCode::_basic_ui_StatusBar_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/components/blind/ui/Card.qml"), &QmlCacheGeneratedCode::_components_blind_ui_Card_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/components/light/ui/Button.qml"), &QmlCacheGeneratedCode::_components_light_ui_Button_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/main.qml"), &QmlCacheGeneratedCode::__main_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/Notification.qml"), &QmlCacheGeneratedCode::_basic_ui_Notification_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/components/blind/ui/Button.qml"), &QmlCacheGeneratedCode::_components_blind_ui_Button_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/components/light/ui/CardColor.qml"), &QmlCacheGeneratedCode::_components_light_ui_CardColor_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/integrations/homeassistant/Light.qml"), &QmlCacheGeneratedCode::_integrations_homeassistant_Light_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/integrations/homeassistant/homeassistant.qml"), &QmlCacheGeneratedCode::_integrations_homeassistant_homeassistant_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/CustomButtonState.qml"), &QmlCacheGeneratedCode::_basic_ui_CustomButtonState_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/pages/settings.qml"), &QmlCacheGeneratedCode::_basic_ui_pages_settings_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Battery.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Battery_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/CustomSwitch.qml"), &QmlCacheGeneratedCode::_basic_ui_CustomSwitch_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/integrations/integration.qml"), &QmlCacheGeneratedCode::_integrations_integration_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/MainContainer.qml"), &QmlCacheGeneratedCode::__MainContainer_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/ChargingScreen.qml"), &QmlCacheGeneratedCode::_basic_ui_ChargingScreen_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Softwareupdate.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Softwareupdate_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/NotificationDrawer.qml"), &QmlCacheGeneratedCode::_basic_ui_NotificationDrawer_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/ClosingScreen.qml"), &QmlCacheGeneratedCode::_basic_ui_ClosingScreen_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/LoadingScreen.qml"), &QmlCacheGeneratedCode::_basic_ui_LoadingScreen_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/Devices.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_Devices_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/scripts/helper.js"), &QmlCacheGeneratedCode::_scripts_helper_js::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/CustomButton.qml"), &QmlCacheGeneratedCode::_basic_ui_CustomButton_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/settings/PowerSaving.qml"), &QmlCacheGeneratedCode::_basic_ui_settings_PowerSaving_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/integrations/homey/homey.qml"), &QmlCacheGeneratedCode::_integrations_homey_homey_qml::unit);
        resourcePathToCachedUnit.insert(QStringLiteral("/basic_ui/pages/device.qml"), &QmlCacheGeneratedCode::_basic_ui_pages_device_qml::unit);
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.version = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
QT_PREPEND_NAMESPACE(qRegisterResourceData)(/*version*/0x01, qt_resource_tree, qt_resource_names, qt_resource_empty_payout);
}
const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url) {
    if (url.scheme() != QLatin1String("qrc"))
        return nullptr;
    QString resourcePath = QDir::cleanPath(url.path());
    if (resourcePath.isEmpty())
        return nullptr;
    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));
    return unitRegistry()->resourcePathToCachedUnit.value(resourcePath, nullptr);
}
}
int QT_MANGLE_NAMESPACE(qInitResources_qml)() {
    ::unitRegistry();
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_qml));
int QT_MANGLE_NAMESPACE(qCleanupResources_qml)() {
    return 1;
}
