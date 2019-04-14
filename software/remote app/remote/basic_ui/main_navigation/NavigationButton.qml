import QtQuick 2.11
import QtGraphicalEffects 1.0

Rectangle {

    property alias mouseArea: mouseArea
    property alias buttonText: buttonText.text
    property alias icon: icon
    property bool selected: false

    id: navigationButton
    width: buttonText.implicitWidth+30
    height: mainNavigation.state == "open" ? 130 : 60
    color: colorBackgroundTransparent
    opacity: selected ? 1 : 0.3

    Behavior on opacity {
        NumberAnimation { duration: 400 }
    }

    Text {
        id: buttonText
        color: colorText
        text: "Navigation item"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 0.8
    }

    Image {
        asynchronous: true
        id: icon
        width: 60
        height: 60
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: buttonText.horizontalCenter
        anchors.bottom: buttonText.top
        anchors.bottomMargin: 10

        ColorOverlay {
            visible: !darkMode
            anchors.fill: parent
            source: parent
            color: colorText
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            var newX = navigationButton.x-navigationButton.width
            if (newX <= 0) {
                newX = 0
            } else if (newX >= (mainNavigationFlickable.contentWidth-460)) {
                newX = mainNavigationFlickable.contentWidth-460;
            }
            mainNavigationFlickable.contentX = newX;
        }
    }
}
