import QtQuick 2.12
import QtQuick.Controls 2.4

Flickable {
    id: itemFlickable
    property string area

    //: Name of the settings page
    property string title: qsTr(area) + translateHandler.emptyString

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
    contentHeight: iconFlow.height + titleContainer.height + 40 //iconFlow.height < 800 - mainNavigation.height - statusBar.height - miniMediaPlayer.height + bottomGradient.height ? iconFlow.height + 40 : iconFlow.height
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

    Rectangle {
        id: titleContainer
        width: parent.width
        height: 200
        color: colorBackground

        Text {
            id: titleText
            color: colorText
            text: qsTr(area) + translateHandler.emptyString
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
            var hasTitle = false;

            for (var i=0; i<config.areas.length; i++) {
                if (config.areas[i].area == area) {
                    for (var k=0; k<entities.loaded_entities.length; k++) {
                        var e = entities.getByType(entities.loaded_entities[k]);

                        for (var j=0; j<e.length; j++) {
                            if (e[j].area == config.areas[i].area) {
                                hasTitle = true;
                            }
                        }

                        if (k>0) {
                            var spacerObj = Qt.createQmlObject('import QtQuick 2.12; Rectangle {color: colorBackgroundTransparent; width: parent.width; height: 40;}', iconFlow, '')
                        }
                        if (hasTitle) {
                            var roomObj = Qt.createQmlObject('import QtQuick 2.12; Text {color: colorText; font.family: "Open Sans"; font.weight: Font.Normal; font.pixelSize: 32; x:10; text: "'+ entities.getSupportedEntityTranslation(entities.loaded_entities[k]) +'";}', iconFlow, "");
                        }
                        hasTitle = false;

                        for (var j=0; j<e.length; j++) {
                            if (e[j].area == config.areas[i].area) {
                                // load entity button
                                var comp = Qt.createComponent("qrc:/components/"+ entities.loaded_entities[k] +"/ui/Button.qml");
                                if (comp.status != Component.Ready) {
                                    console.debug("Error: " + comp.errorString() );
                                }
                                var obj = comp.createObject(iconFlow, {obj: e[j]});

                            }
                        }
                    }
                }
            }
        }
    }
}
