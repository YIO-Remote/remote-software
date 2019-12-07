import QtQuick 2.12
import QtQuick.Controls 2.12

import MediaPlayerUtils 1.0

Item {
    width: parent.width
    height: parent.height

    property string albumId

    Component.onCompleted: {
        console.debug("ALBUM ID " + albumId);
    }

    // include mediaplayer utils
    MediaPlayerUtils {
        id: mediaplayerUtils
    }

    Rectangle {
        anchors.fill: parent
        color: "blue"
    }

}
