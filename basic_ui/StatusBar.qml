/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import Style 1.0

Item {
    id: statusBar
    width: parent.width
    height: 40
    clip: true

//    property alias bg: bg

    Rectangle {
        id: bg
        anchors.fill: parent
        color: Style.colorBackground
        opacity: 0

        Behavior on opacity {
            NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
        }
    }

    Connections {
        target: loader_main.item.mainNavigationSwipeview.currentItem.mainNavigationLoader.item
        ignoreUnknownSignals: true

        onScrollupBegin: {
            bg.opacity = 1;
        }

        onScrolldownBegin: {
            bg.opacity = 0;
        }

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
    property string title: ""

    Text {
        id: titleText
        color: Style.colorText
        text: ""
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        width: parent.width/2
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
        PropertyAnimation { target: timeText; properties: "opacity"; to: 0; easing.type: Easing.OutExpo; duration: 400 }
        PropertyAnimation { target: notificationCount; properties: "anchors.rightMargin"; to: 10-(timeText.width/2); easing.type: Easing.OutExpo; duration: 400 }
    }

    ParallelAnimation {
        id: hideTitle
        running: false

        PropertyAnimation { target: titleText; properties: "opacity"; to: 0; easing.type: Easing.InExpo; duration: 400 }
        PropertyAnimation { target: titleText; properties: "y"; to: 60; easing.type: Easing.OutExpo; duration: 400 }
        PropertyAnimation { target: timeText; properties: "opacity"; to: 1; easing.type: Easing.OutExpo; duration: 400 }
        PropertyAnimation { target: notificationCount; properties: "anchors.rightMargin"; to: 10; easing.type: Easing.OutExpo; duration: 400 }
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

    property alias timeText: timeText

    Text { // time in the middle
        id: timeText
        color: Style.colorText
        text: "22:00"
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTIFICATION ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: notificationCount
        width: 12
        height: width
        radius: width/2
        color: Style.colorRed
        visible: notifications.list.length > 0 ? true : false
        anchors.right: timeText.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
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
        for (var i=0; i<integrations.list.length; i++) {
            integrations.list[i].connecting.connect(loadingIconON);
            integrations.list[i].disconnected.connect(loadingIconOFF);
            integrations.list[i].connected.connect(loadingIconOFF);
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
        anchors.right: batteryIcon.left
        anchors.rightMargin: 10
//        fillMode: Image.PreserveAspectFit
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
            color: battery_level >= 0.200 ? Style.colorLine : Style.colorRed
        }
        // battery frame
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: 30
            height: 18
            color: Style.colorBackgroundTransparent
            radius: 4
            border.width: 2
            border.color: Style.colorLine
        }
        // battery notch
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            width: 8
            height: 10
            color: Style.colorBackgroundTransparent
            radius: 2
            border.width: 2
            border.color: Style.colorLine
        }
    }

    Text {
        id: batteryPercentText
        color: Style.colorText
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

    MouseArea {
        anchors.fill: parent

        onClicked: {
            loader_main.item.mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = 0;
        }

        onPressAndHold: {
            haptic.playEffect("press");
            loader_second.setSource("qrc:/basic_ui/Profiles.qml");
        }
    }

}
