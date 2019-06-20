import QtQuick 2.11

Item {
    property var entities
    property var componentIntegration: ({})

    Component.onCompleted: {
        // load the hub integrations
        var comp;
        for (var i=0; i<config.integration.length; i++) {
            comp = Qt.createComponent("qrc:/components/light/integrations/"+ config.integration[i].type +".qml");
            if (comp.status !== Component.Ready) {
                console.debug("Error: " + comp.errorString() );
            }
            componentIntegration[config.integration[i].type] = comp.createObject(applicationWindow);
        }
    }
}
