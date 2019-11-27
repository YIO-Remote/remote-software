import QtQuick 2.11
import QtQuick.Controls 2.5

Flickable {
    id: itemFlickable

    //: Name of the settings page
    property string title: qsTr("Favorites") + translateHandler.emptyString

    property var favoritesModel: config.getProfileFavorites() //config.ui_config.profiles[config.profile].favorites

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
    contentHeight: listView.height + titleContainer.height + mainNavigation.height + 20
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
        id: titleContainer
        width: parent.width
        height: 240

        Text {
            id: titleText
            color: colorText
            text: title
            anchors.centerIn: parent
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 60
            lineHeight: 1
        }
    }

    // ENTITIES
    ListView {
        id: listView
        width: parent.width
        height: contentHeight
        anchors.top: titleContainer.bottom
        interactive: false
        spacing: 10

        model: favoritesModel
        delegate: entityDelegate
    }


    // LISTVIEW DELEGATE
    Component {
        id: entityDelegate

        Loader {
            id: entityLoader
            width: 460
            height: 125
            anchors.horizontalCenter: parent.horizontalCenter

            Component.onCompleted: {
                var e = entities.get(favoritesModel[index]);
                this.setSource("qrc:/components/"+ e.type +"/ui/Button.qml", { "obj": e });
            }
        }
    }

}
