import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI

Flickable {
    id: itemFlickable

    width: parent.width
    height: parent.height
    maximumFlickVelocity: 4000
    flickDeceleration: 2000
    clip: true
    contentHeight: settingsPage.height
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    ScrollBar.vertical: ScrollBar {
        opacity: 0.5
    }

    BasicUI.Settings {
        id: settingsPage
    }
}
