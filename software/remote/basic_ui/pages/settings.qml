import QtQuick 2.12
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI

Flickable {
    id: itemFlickable

    //: Name of the settings page
    property string title: qsTr("Settings") + translateHandler.emptyString

    signal scrolledUp()
    signal scrolledDown()

    onContentYChanged: {
        if (contentY > 130) {
            scrolledUp();
        } else {
            scrolledDown();
        }
    }

    width: parent.width
    height: parent.height
    maximumFlickVelocity: 6000
    flickDeceleration: 1000
    contentHeight: settingsPage.height + 40
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    onFlickStarted: {
        loader_main.item.mainNavigation.y = 800;
    }

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
