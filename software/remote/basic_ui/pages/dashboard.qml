import QtQuick 2.11
import QtQuick.Controls 2.4

Flickable {
    id: itemFlickable

    //: Name of the settings page
    property string title: qsTr("Favorites") + translateHandler.emptyString

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
    contentHeight: iconFlow.height + 40 //iconFlow.height < 800 - mainNavigation.height - statusBar.height - miniMediaPlayer.height + bottomGradient.height ? iconFlow.height + 40 : iconFlow.height
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

    property alias iconFlow: iconFlow

    Column {
        id: iconFlow
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Component.onCompleted: {
            for (var i =0; i < entities.list.length; i++) {
                if (entities.list[i].favorite) {
                    // load entity button
                    var comp = Qt.createComponent("qrc:/components/"+ entities.list[i].type +"/ui/Button.qml");
                    if (comp.status != Component.Ready) {
                        console.debug("Error: " + comp.errorString() );
                    }
                    var obj = comp.createObject(iconFlow, { obj: entities.list[i] });
                }
            }
        }
    }
}
