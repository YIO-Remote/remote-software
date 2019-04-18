import QtQuick 2.11

Item {
    id: lightComponent

    property var lightComponentIntegration: ({})

    Component.onCompleted: {
        // load the hub integrations
        for (var i=0; i<config.integration.length; i++) {
            var comp = Qt.createComponent("qrc:/components/light/"+ config.integration[i].type +".qml");
            lightComponentIntegration[config.integration[i].type] = comp.createObject(applicationWindow);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //    function loadComponent(target, entity_id, state) {
    //        var comp = Qt.createComponent("qrc:/components/light/Button.qml");
    //        var obj = comp.createObject(target, {"entity_id": entity_id, "lstate": state});
    //    }
}
