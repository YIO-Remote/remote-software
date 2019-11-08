import QtQuick 2.0

Item {
    id: imageLoader
    property string url
    property string prevUrl

    onUrlChanged: {
        if (url != prevUrl) {
            image2.opacity = 0;
            image2.source = url;
            prevUrl = url;
        }
    }

    Image {
        id: image1
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: true

        onStatusChanged: {
            if (image1.status == Image.Ready) {
                image2.opacity = 0;
            }
        }
    }

    Image {
        id: image2
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        opacity: 0

        Behavior on opacity {
            NumberAnimation { duration: 800; easing.type: Easing.OutExpo }
        }

        onStatusChanged: {
            if (image2.status == Image.Ready) {
                image2.opacity = 1;
            }
        }

        onOpacityChanged: {
            if (image2.opacity == 1) {
                image1.source = url;
            }
        }
    }
}
