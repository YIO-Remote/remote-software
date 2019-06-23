import QtQuick 2.11
import QtQuick.Controls 2.4

import Launcher 1.0

import "settings" as Settings

Item {
    width: parent.width-20
    height: settingsFlow.height
    anchors.horizontalCenter: parent.horizontalCenter

    Launcher {
        id: settingsLauncher
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SETTINGS BLOCKS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Flow {
        id: settingsFlow
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 40

        Settings.Display {}

        Settings.Softwareupdate {}

        Settings.PowerSaving {}

        Settings.Languages {}

        Settings.Hubs {
            visible: Object.keys(integration).length == 0 ? false : true
        }

        Settings.Devices {
            visible: Object.keys(device).length == 0 ? false : true
        }

        Settings.Battery {}

        Settings.Network {}

        Settings.System {}

    }
}
