import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Flickable {
    id: itemFlickable
    property string area

    //: Name of the settings page
    property string title: qsTr(area) + translateHandler.emptyString

    // image background handling
    property string img_url

    // get the URL from config JSON

    Component.onCompleted: {
        img_url = Qt.binding(function () {
            for (var i=0; i<config.read.ui_config.profiles[config.profile].config.length; i++) {
                if (config.read.ui_config.profiles[config.profile].config[i].friendly_name == area) {
                    if (config.read.ui_config.profiles[config.profile].config[i].image) {
                        topImage.visible = true;
                        return "file://" + config.read.ui_config.profiles[config.profile].config[i].image;
                    } else {
                        topImage.visible = false;
                        return "";
                    }
                }
            }
        });
    }

    signal scrolledUp()
    signal scrolledDown()
    signal scrollupBegin()
    signal scrolldownBegin()

    onContentYChanged: {
        if (contentY > 10) {
            scrollupBegin();
        } else {
            scrolldownBegin()
        }

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
    contentHeight: iconFlow.height + titleContainer.height + mainNavigation.height + 20 //iconFlow.height < 800 - mainNavigation.height - statusBar.height - miniMediaPlayer.height + bottomGradient.height ? iconFlow.height + 40 : iconFlow.height
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    Behavior on contentY {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    ScrollBar.vertical: ScrollBar {
        opacity: 0.5
    }

    Item {
        id: topImage
        width: parent.width
        height: 270
        anchors.top: parent.top

        Rectangle {
            id: comp
            anchors.fill: parent
            color: colorBackground

            Image {
                id: bgImage
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                source: img_url
            }

            Rectangle {
                id: gradient
                width: parent.width
                height: 80
                anchors.bottom: parent.bottom

                LinearGradient {
                        anchors.fill: parent
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: colorBackgroundTransparent }
                            GradientStop { position: 1.0; color: colorBackground }
                        }
                    }
            }

            Blend {
                anchors.fill: bgImage
                source: bgImage
                foregroundSource: gradient
                mode: "multiply"
            }
        }

        Image {
            id: noise
            anchors.fill: parent
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
            source: "qrc:/images/mini-music-player/noise.png"
        }

        Blend {
            anchors.fill: comp
            source: comp
            foregroundSource: noise
            mode: "multiply"
        }

        Rectangle {
            id: overlay
            anchors.fill: noise
            color: colorBackground
            opacity: 0.7
        }
    }

    Item {
        id: titleContainer
        width: parent.width
        height: 240

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

//    Column {
//        id: iconFlow
//        width: parent.width
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: titleContainer.bottom
//        spacing: 10

//        Component.onCompleted: {
//            var hasTitle = false;

//            for (var i=0; i<config.read.areas.length; i++) {
//                if (config.read.areas[i].area == area) {
//                    for (var k=0; k<entities.loaded_entities.length; k++) {
//                        var e = entities.getByType(entities.loaded_entities[k]);

//                        for (var j=0; j<e.length; j++) {
//                            e[j].friendly_name;
//                            if (e[j].area == area) {
//                                hasTitle = true;
//                            }
//                        }

//                        if (k>0) {
//                            var spacerObj = Qt.createQmlObject('import QtQuick 2.11; Rectangle {color: colorBackgroundTransparent; width: parent.width; height: 40;}', iconFlow, '')
//                        }
//                        if (hasTitle) {
//                            var roomObj = Qt.createQmlObject('import QtQuick 2.11; Text {color: colorText; font.family: "Open Sans"; font.weight: Font.Normal; font.pixelSize: 32; x:10; text: "'+ entities.getSupportedEntityTranslation(entities.loaded_entities[k]) +'";}', iconFlow, "");
//                        }
//                        hasTitle = false;

//                        for (var j=0; j<e.length; j++) {
//                            if (e[j].area == area) {
//                                // load entity button
//                                var comp = Qt.createComponent("qrc:/components/"+ entities.loaded_entities[k] +"/ui/Button.qml");
//                                if (comp.status != Component.Ready) {
//                                    console.debug("Error: " + comp.errorString() );
//                                }
//                                var obj = comp.createObject(iconFlow, {obj: e[j]});

//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
}
