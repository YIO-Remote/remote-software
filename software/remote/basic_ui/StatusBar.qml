import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

Item {
    id: statusBar
    width: parent.width
    height: 40
    clip: true

    Rectangle {
        anchors.fill: parent
        color: colorBackground
    }

    Connections {
        target: loader_main.item.mainNavigationSwipeview.currentItem.mainNavigationLoader.item
        onScrolledUp: {
            title = loader_main.item.mainNavigationSwipeview.currentItem.mainNavigationLoader.item.title;
        }
        onScrolledDown: {
            title = "";
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // TITLE
    //////////////////////////////////////////////////////////////////////////////////////////////////
    property string title: "Favorites"

    Text { // time in the left corner
        id: titleText
        color: colorText
        text: "Favorites"
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: 10
        y: 2
        font.family: "Open Sans"
        font.weight: Font.Bold
        font.styleName: "Bold"
        font.pixelSize: 22
        font.capitalization: Font.AllUppercase
        lineHeight: 1
    }

    ParallelAnimation {
        id: showTitle
        running: false

        PropertyAnimation { target: titleText; properties: "opacity"; to: 1; easing.type: Easing.OutExpo; duration: 400 }
        PropertyAnimation { target: titleText; properties: "y"; to: 2; easing.type: Easing.OutExpo; duration: 400 }
    }

    ParallelAnimation {
        id: hideTitle
        running: false

        PropertyAnimation { target: titleText; properties: "opacity"; to: 0; easing.type: Easing.InExpo; duration: 400 }
        PropertyAnimation { target: titleText; properties: "y"; to: 60; easing.type: Easing.InExpo; duration: 400 }
    }

    Connections {
        target: hideTitle
        onFinished: {
            titleText.text = title;
        }
    }

    onTitleChanged: {
        if (title == "") {
            hideTitle.start();
        } else {
            titleText.text = title;
            showTitle.start();
        }
    }

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
        anchors.right: batteryIcon.left
        anchors.rightMargin: 10
        anchors.verticalCenter: statusBar.verticalCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTIFICATION ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////

    function isThereAnError() {
        var returning = false;
        for (var i=0; i<notifications.length; i++) {
            if (notifications[i].type == "error") {
                returning = true;
            }
        }
        return returning;
    }

    Rectangle {
        id: notificationCount
        width: 26
        height: width
        radius: width/2
        color: isThereAnError() ? colorRed : colorHighlight
        visible: notifications.length > 0 ? true : false
        anchors.right: timeText.left
        anchors.rightMargin: 10
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
    function loadingIconON() {
        loadingIcon.opacity = 1;
        loadingIcon.visible = true;
    }

    function loadingIconOFF() {
        loadingIcon.opacity = 0;
        loadingIcon.visible = false;
    }

    Component.onCompleted: {
        for (var key in integration) {
            if (integration.hasOwnProperty(key)) {
                integration[key].obj.connecting.connect(loadingIconON);
                integration[key].obj.disconnected.connect(loadingIconOFF);
                integration[key].obj.connected.connect(loadingIconOFF);
            }
        }
    }

    Image {
        asynchronous: true
        id: loadingIcon
        width: 26
        height: 26
        opacity: 0
        visible: false
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: notificationCount.visible ? notificationCount.left : timeText.left
        anchors.rightMargin: 10
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/statusbar/statusbar-loader.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: loadingIcon.visible
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

