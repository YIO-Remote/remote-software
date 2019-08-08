import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: notificationDrawerContent
    width: parent.width
    height: parent.height

    // TITLE

    Text {
        id: title
        text: qsTr("Notifications") + translateHandler.emptyString
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 40

        color: colorText
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 60
        lineHeight: 1
    }

    // HERE COMES THE NOTIFICATIONS

    Component {
        id: notificationDelegate

        BasicUI.Notification {
            _state: "permanent"
            type: notifications.list[index].error
            text: notifications.list[index].text
            action: notifications.list[index].action
            actionlabel: notifications.list[index].actionlabel
            timestamp: notifications.list[index].timestamp
            idN: notifications.list[index].id
        }
    }


    ListView {
        id: notificationsListView
        width: parent.width
        height: parent.height - title.height - 70
        anchors.top: title.bottom
        anchors.topMargin: 30
        maximumFlickVelocity: 4000
        flickDeceleration: 2000
//        clip: true
        flickableDirection: Flickable.VerticalFlick
        contentHeight: count * notificationDelegate.height
        spacing: 10

        ScrollBar.vertical: ScrollBar {
            opacity: 0.5
        }

        model: notifications.list

        delegate: notificationDelegate
    }
}
