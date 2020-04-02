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
import Style 1.0

import Battery 1.0
import Haptic 1.0

Item {
    id: statusBar
    width: parent.width; height: 40
    clip: true

    property alias bg: bg

    Rectangle {
        id: bg
        anchors.fill: parent
        color: Style.color.background
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
        color: Style.color.text
        text: ""
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        y: 2
        width: parent.width/2; height: parent.height
        anchors { left: parent.left; leftMargin: 10 }
        font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 22; capitalization: Font.AllUppercase }
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
        color: Style.color.text
        text: "22:00"
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 28 }
        lineHeight: 1
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTIFICATION ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: notificationCount
        width: 12; height: 12
        radius: 6
        color: Style.color.red
        visible: notifications.list.length > 0 ? true : false
        anchors { right: timeText.left; rightMargin: 10; verticalCenter: parent.verticalCenter }
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
        width: 26; height: 26
        opacity: 0
        visible: false
        anchors { verticalCenter: parent.verticalCenter; right: batteryIcon.left; rightMargin: 10 }
        source: "qrc:/images/statusbar/statusbar-loader.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: loadingIcon.visible
            loops: Animation.Infinite
            from: 0; to: 360
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
        anchors { right: batteryPercentText.left; rightMargin: 7; verticalCenter: statusBar.verticalCenter }

        // battery fill
        Rectangle {
            id: icon_battery
            width: Battery.level/100*20; height: 6
            color: Battery.level >= 20 ? Style.color.line : Style.color.red
            anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 5 }
        }
        // battery frame
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: 30; height: 18
            color: Style.color.backgroundTransparent
            radius: 4
            border { width: 2; color: Style.color.line }
        }
        // battery notch
        Rectangle {
            width: 8; height: 10
            color: Style.color.backgroundTransparent
            radius: 2
            border { width: 2; color: Style.color.line }
            anchors { verticalCenter: parent.verticalCenter; right: parent.right }
        }
    }

    Text {
        id: batteryPercentText
        color: Style.color.text
        text: Math.round(Battery.level) + "%"
        verticalAlignment: Text.AlignVCenter
        height: parent.height
        anchors { right: profileIcon.left; rightMargin: 10; verticalCenter: statusBar.verticalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
        lineHeight: 1
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PROFILE ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: profileIcon
        width: Object.keys(config.profiles).length > 1 ? 26 : 0; height: width
        radius: 13
        color: Style.color.medium
        visible: Object.keys(config.profiles).length > 1

        anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 10 }

        Text {
            color: Style.color.text
            text: config.getProfile().name.substring(0,1);
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            anchors.centerIn: parent
            font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 14 }
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            loader_main.item.mainNavigationSwipeview.currentItem.mainNavigationLoader.item._contentY = 0;
        }
    }

    MouseArea {
        width: profileIcon.width + 20
        height: width

        anchors.centerIn: profileIcon

        onClicked: {
            Haptic.playEffect(Haptic.Click);
            loader_second.setSource("qrc:/basic_ui/Profiles.qml");
            loader_second.active = true;
        }
    }
}
