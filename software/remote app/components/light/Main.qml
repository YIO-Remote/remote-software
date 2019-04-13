import QtQuick 2.11

Item {
    id: lightComponent

    property var entities
    property var lightComponentIntegration

    Component.onCompleted: {
        // load the hub integration
        var comp = Qt.createComponent("qrc:/components/light/"+ config.integration +".qml");
        lightComponentIntegration = comp.createObject(applicationWindow);
    }
}
