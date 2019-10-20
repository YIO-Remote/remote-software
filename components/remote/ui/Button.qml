import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/components" as Comp

Comp.ButtonBase {
    id: remoteButton
    icon: "\uE909"
    cardLoader.source: getSource()

    function getSource() {
        if (remoteButton.state != "open") {
            return "";
        } else {
            return "qrc:/components/remote/ui/" + integrations.getType(obj.integration) + "/Card.qml";
        }
    }
}

