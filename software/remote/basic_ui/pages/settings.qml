import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI

Flickable {
    id: itemFlickable

    width: parent.width
    height: parent.height
    maximumFlickVelocity: 6000
    flickDeceleration: 1000
    contentHeight: settingsPage.height
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    Behavior on contentY {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    ScrollBar.vertical: ScrollBar {
        opacity: 0.5
    }

    BasicUI.Settings {
        id: settingsPage
    }
}
