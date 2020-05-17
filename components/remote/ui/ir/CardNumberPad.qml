import QtQuick 2.12
import QtQuick.Layouts 1.3
import Style 1.0
import "qrc:/basic_ui" as BasicUI

Item {
    id: container
    width: parent.width
    anchors.horizontalCenter: parent.horizontalCenter

    Connections {
        target: volume
        onVolumeChanged: {
            console.log('Volume Changed', volume.volumeUp, volume.volumePosition)
            if (volume.volumeUp) {
                if (volume.state !== "visible") {
//                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                var newvolume = volume.volumePosition + 2;
                if (newvolume > 100) newvolume = 100;
//                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            } else {
                if (volume.state !== "visible") {
//                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                newvolume = volume.volumePosition - 2;
                if (newvolume < 0) newvolume = 0;
//                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            }
        }
    }

    GridLayout {
        id: numberGrid
        columns: 3
        width: parent.width * 0.8
        height: parent.height * 0.7
        anchors.horizontalCenter: parent.horizontalCenter

        Button {
            title: "1"
            Layout.alignment: Qt.AlignHCenter
            mouseArea.onPressed: {
                volume.timer.start();
            }
            mouseArea.onReleased: {
                volume.timer.stop();
            }
        }
        Button {
            title: "2"
            Layout.alignment: Qt.AlignHCenter
            mouseArea.onClicked: {
                volume.volumeUp = !volume.volumeUp;
            }
        }
        Button {
            title: "3"
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            title: "4"
            Layout.alignment: Qt.AlignHCenter
        }
        Button {
            title: "5"
            Layout.alignment: Qt.AlignHCenter
        }
        Button {
            title: "6"
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            title: "7"
            Layout.alignment: Qt.AlignHCenter
        }
        Button {
            title: "8"
            Layout.alignment: Qt.AlignHCenter
        }
        Button {
            title: "9"
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            title: "0"
            Layout.alignment: Qt.AlignHCenter
            Layout.columnSpan: 3
        }
    }

    GridLayout {
        id: colourGrid
        columns: 4
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: parent.height * 0.2

        Button {
            Layout.alignment: Qt.AlignHCenter
            color: Style.color.green
            width: (parent.width / 4) * 0.5
            height: width
            border.width: 0
            mouseArea.onPressed: {
                this.color = Style.color.greenTint
            }
            mouseArea.onReleased: {
                this.color = Style.color.green
            }
        }
        Button {
            Layout.alignment: Qt.AlignHCenter
            color: Style.color.red
            width: (parent.width / 4) * 0.5
            height: width
            border.width: 0
            mouseArea.onPressed: {
                this.color = Style.color.redTint
            }
            mouseArea.onReleased: {
                this.color = Style.color.red
            }
        }
        Button {
            Layout.alignment: Qt.AlignHCenter
            color: Style.color.orange
            width: (parent.width / 4) * 0.5
            height: width
            border.width: 0
            mouseArea.onPressed: {
                this.color = Style.color.orangeTint
            }
            mouseArea.onReleased: {
                this.color = Style.color.orange
            }
        }
        Button {
            Layout.alignment: Qt.AlignHCenter
            color: Style.color.blue
            width: (parent.width / 4) * 0.5
            height: width
            border.width: 0
            mouseArea.onPressed: {
                this.color = Style.color.blueTint
            }
            mouseArea.onReleased: {
                this.color = Style.color.blue
            }
        }
    }
}
