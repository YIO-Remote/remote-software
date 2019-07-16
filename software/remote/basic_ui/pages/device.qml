import QtQuick 2.11
import QtQuick.Controls 2.4

Flickable {
    id: itemFlickable
    property string type

    //: Name of the settings page
    property string title: qsTr(findType(type)) + translateHandler.emptyString

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
    contentHeight: iconFlow.height + 200 + 40 //iconFlow.height < 800 - mainNavigation.height - statusBar.height - miniMediaPlayer.height + bottomGradient.height ? iconFlow.height + 40 : iconFlow.height
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

    function findType(type) {
        for (var i=0; i<entities.supported_entities.length; i++) {
            if (type == entities.supported_entities[i]) {
                return supported_entities_translation[i];
            }
        }
    }

    Rectangle {
        id: titleContainer
        width: parent.width
        height: 200
        color: colorBackground

        Text {
            id: titleText
            color: colorText
            text: qsTr(findType(type)) + translateHandler.emptyString
            anchors.centerIn: parent
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 60
            lineHeight: 1
        }
    }

    Column {
        id: iconFlow
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titleContainer.bottom
        spacing: 10

        Component.onCompleted: {
            var e = entities.getByType(type);

            for (var i=0; i<e.length; i++) {
                var comp = Qt.createComponent("qrc:/components/"+ type +"/ui/Button.qml");
                if (comp.status != Component.Ready) {
                    console.debug("Error: " + comp.errorString() );
                }
                var obj = comp.createObject(iconFlow, {obj: e[i]});
            }

        }
    }
}
