import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

Item {
    id: statusBar
    width: parent.width
    height: 40

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // TIME
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Timer { // timer that updates the time and date
        id: timer
        interval: 100
        repeat: true
        running: true

        onTriggered:
        {
            timeText.text =  Qt.formatDateTime(new Date(),"hh:mm")
        }
    }


    Text { // time in the left corner
        id: timeText
        color: colorText
        text: "22:00"
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: statusBar.verticalCenter
        anchors.top: parent.top
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTIFICATION ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Rectangle {
        id: notificationCount
        width: 26
        height: width
        radius: width/2
        color: colorHighlight
        visible: notifications.length > 0 ? true : false
        anchors.left: timeText.right
        anchors.leftMargin: 10
        anchors.verticalCenter: statusBar.verticalCenter

        Text {
            id: notificationCountText
            color: colorText
            text: notifications.length
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            font.family: "Open Sans"
            font.weight: Font.Bold
            font.styleName: "Bold"
            font.pixelSize: 20
            lineHeight: 1
        }

        MouseArea {
            width: parent.width + 60
            height: width
            anchors.centerIn: parent

            onClicked: {
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // LOADING ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Image {
        asynchronous: true
        id: loadingIcon
        width: 26
        height: 26
        opacity: connecting ? 1 : 0
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: batteryIcon.left
        anchors.rightMargin: 20
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/statusbar/statusbar-loader.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: connecting
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 2000
        }

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.InExpo; duration: 300 }
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Rectangle {
        id: batteryIcon
        width: 36
        anchors.right: batteryPercentText.left
        anchors.rightMargin: 5
        anchors.verticalCenter: statusBar.verticalCenter

        // battery fill
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            id: icon_battery
            width: battery_level*20
            height: 6
            color: battery_level >= 0.200 ? colorLine : colorRed
        }
        // battery frame
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: 30
            height: 18
            color: colorBackgroundTransparent
            radius: 4
            border.width: 2
            border.color: colorLine
        }
        // battery notch
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            width: 8
            height: 10
            color: colorBackgroundTransparent
            radius: 2
            border.width: 2
            border.color: colorLine
        }
    }

    Text {
        id: batteryPercentText
        color: colorText
        text: Math.round(battery_level * 100) + "%"
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: statusBar.verticalCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
    }
}

