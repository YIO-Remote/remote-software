import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import Haptic 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: weatherList
    width: parent.width
    height: parent.height
    color: colorDark

    /////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS (with long press detection)
    /////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: true

        onButtonPress: {
            if (standbyControl.mode === "on" || standbyControl.mode === "dim") {
                switch (button) {
                case "top right":
                    weatherButton.state = "closed"
                    loader_main.state = "visible"
                    break;
                }
            }
        }
        onButtonRelease: {
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Header type title
    /////////////////////////////////////////////////////////////////////////////////////
    Item {
        id: location;
        width: parent.width
        height: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 10
        Text {
            color: colorText
            text: obj.friendly_name
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            anchors { centerIn: parent }
            font.family: "Open Sans"
            font.weight: Font.Bold
            font.pixelSize: 32
            lineHeight: 1
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Items List
    /////////////////////////////////////////////////////////////////////////////////////
    Component {
        id: listDelegate
        Item {
            width: parent.width
            height: 120
            Rectangle {
                id: rect
                width: parent.width
                height: parent.height
                visible: true
                color: colorMedium
                radius: cornerRadius

                Text {
                    id: date
                    width: 30
                    anchors { left: parent.left; top:parent.top; leftMargin: 10; verticalCenter: parent.verticalCenter }
                    verticalAlignment: Text.AlignVCenter
                    color: colorText
                    text: model.date
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Image {
                    id: image
                    asynchronous: true
                    width: 80
                    height: 80
                    fillMode: Image.PreserveAspectFit
                    anchors { left: date.right; top:parent.top; topMargin: 0 }
                    source: model.imageurl;
                }
                Text {
                    id: desc
                    anchors { left: image.right; top:parent.top; topMargin: 20 }
                    color: colorText
                    text: model.description
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Text {
                    id: temp
                    anchors { right: parent.right; top:parent.top; topMargin: 20; rightMargin: 10 }
                    color: colorText
                    text: model.temp
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }

                Image {
                    id: imgRain
                    width: 24
                    height: 24
                    anchors { left: image.left; leftMargin: 20; bottom: parent.bottom; bottomMargin: 20 }
                    asynchronous: true
                    visible: model.rain !== ""
                    source: "qrc:/components/weather/images/rain.png"
                }
                Text {
                    id: rain
                    anchors { left: imgRain.right; leftMargin:10; bottom: parent.bottom; bottomMargin: 15 }
                    color: colorText
                    text: model.rain
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Image {
                    id: imgSnow
                    width: 24
                    height: 24
                    asynchronous: true
                    anchors { left: image.left; leftMargin: 20; bottom: parent.bottom; bottomMargin: 20 }
                    visible: model.snow !== ""
                    source: "qrc:/components/weather/images/snow.png"
                }
                Text {
                    id: snow
                    anchors { left: imgSnow.right; leftMargin:10; bottom:parent.bottom; bottomMargin: 15 }
                    color: colorText
                    text: model.snow
                    visible: model.snow
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Image {
                    id: imgWind
                    width: 24
                    height: 24
                    asynchronous: true
                    anchors { left: image.left; leftMargin: 150; bottom: parent.bottom; bottomMargin: 20 }
                    visible: model.wind !== ""
                    source: "qrc:/components/weather/images/wind.png"
                }
                Text {
                    id: wind
                    anchors { left: imgWind.right; leftMargin:10; bottom :parent.bottom; bottomMargin: 15 }
                    color: colorText
                    text: model.wind
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Text {
                    id: humidity
                    anchors { right: parent.right; rightMargin: 10; bottom : parent.bottom; bottomMargin: 15 }
                    color: colorText
                    text: model.humidity
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
            }
        }
    }
    ListView {
        id: listView
        width: parent.width
        height: parent.height - location.bottom - 40
        spacing: 10
        orientation: ListView.Vertical
        flickableDirection: Flickable.VerticalFlick
        anchors {
            left: parent.left; top: location.bottom;
            right: parent.right; bottom: parent.bottom;
            margins: 10
        }
        clip: true
        model: obj.forecast
        delegate: listDelegate
    }


    /////////////////////////////////////////////////////////////////////////////////////
    // Close Button
    /////////////////////////////////////////////////////////////////////////////////////
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

        ColorOverlay {
            visible: !darkMode
            anchors.fill: parent
            source: parent
            color: colorText
            antialiasing: true
        }

        MouseArea {
            width: parent.width + 60
            height: parent.height + 60
            anchors.centerIn: parent

            onClicked: {
                Haptic.playEffect("click");
                weatherButton.state = "closed"
            }
        }
    }
}
