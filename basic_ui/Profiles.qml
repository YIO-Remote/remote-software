import QtQuick 2.11
import QtQuick.Controls 2.5

Rectangle {
    id: profiles
    width: 480
    height: 0
    color: colorHighlight2
    radius: cornerRadius
    anchors.bottom: parent.bottom

    Component.onCompleted: {
        parent.width = width;
        parent.height = height;
        parent.anchors.bottom = parent.parent.bottom;

        for (var value in config.ui_config.profiles) {
            var v = config.ui_config.profiles[value];
            v["id"] = value
            profileModel.append(v);
        }

        for (var i=0; i<profileModel.count; i++) {
            if (profileModel.get(i).id === config.profile) {
                profileModel.move(i, 0, 1);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"


    states: [
        State { name: "closed";
            PropertyChanges {target: profiles; height: 0 }
            PropertyChanges {target: parent; height: 0 }
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State { name: "open";
            when: parent.status === Loader.Ready
            PropertyChanges {target: profiles; height: 670 }
            PropertyChanges {target: parent; height: 670 }
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]
    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: profiles; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: parent; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
                }
                PropertyAction { target: loader_second; property: "source"; value: "" }
            }
        },
        Transition {to: "open";
            PropertyAnimation { target: profiles; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
            PropertyAnimation { target: parent; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: title
        color: colorText
        text: qsTr("Profiles") + translateHandler.emptyString
        anchors { top: parent.top; topMargin: 30; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 60 }
        lineHeight: 0.9
    }

    // PROFILES
    ListView {
        id: profileListView
        width: parent.width-60
        height: parent.height
        anchors.top: title.bottom
        anchors.topMargin: 70
        anchors.left: parent.left
        anchors.leftMargin: 30
        spacing: 30

        model: profileModel
        delegate: profileDelegate

        move: Transition {
                NumberAnimation { properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            }
    }

    // MODEL
    ListModel {
        id: profileModel
    }

    // LISTVIEW DELEGATE
    Component {
        id: profileDelegate

        Item {
            width: 380
            height: id == config.profile ? 100 : 70

            Rectangle {
                width: parent.width
                height: 2
                color: colorLight
                visible: id == config.profile ? true : false
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                id: img
                width: 70
                height: 70
                radius: width/2
                color: id == config.profile ? colorText : colorHighlight1
                anchors.top: parent.top
                anchors.left: parent.left

                Text {
                    anchors.centerIn: parent
                    color: id == config.profile ? colorBackground : colorText
                    text: name.substring(0,1);
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font {family: "Open Sans Bold"; pixelSize: 40 }
                }
            }

            Text {
                color: id == config.profile ? colorText : colorHighlight1
                text: name
                anchors { verticalCenter: img.verticalCenter; left: img.right; leftMargin: 20 }
                font {family: "Open Sans Regular"; pixelSize: 27 }
                lineHeight: 0.9
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");
                    config.profile = id;
                    profileModel.move(index, 0, 1);
                }
            }
        }
    }

    Text {
        id: smallText
        color: colorText
        opacity: 0.5
        text: qsTr("To edit your profiles, use the web configurator tool in settings.") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        width: parent.width - 60
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
    }

    Image {
        id: closeButton
        asynchronous: true
        width: 30
        height: 30
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/components/close_button.png"
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20

        MouseArea {
            width: parent.width + 60
            height: parent.height + 60
            anchors.centerIn: parent

            onClicked: {
                haptic.playEffect("click");
                profiles.state = "closed"
            }
        }
    }

}
