import QtQuick 2.11
import QtQuick.Controls 2.5

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: languageSection
    width: parent.width
    height: header.height + section.height + 20

    function getLanguage(id) {
        for (var i=0; i<translations.length; i++) {
            if (translations[i].id == id) {
                return i;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LANGUAGES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: header
        color: colorText
        text: qsTr("Languages") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: 74
        radius: cornerRadius
        color: colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        clip: true

        Behavior on height {
            PropertyAnimation {easing.type: Easing.OutExpo; duration: 300}
        }

        ListView {
            id: languageListView
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: 300
            model: translations
            preferredHighlightBegin: height / 2 - 37
            preferredHighlightEnd: height / 2 + 37
            highlightMoveDuration: 50
            highlightRangeMode: ListView.StrictlyEnforceRange
            interactive: section.height == 300 ? true : false

            currentIndex: getLanguage(config.settings.language);

            focus: true

            delegate:
                Rectangle {
                width: parent.width
                height: 74
                color: colorBackgroundTransparent

                Text {
                    text: translations[index].name
                    color: colorText
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (section.height != 300) {
                            section.height = 300
                        } else {
                            languageListView.currentIndex = index
                            section.height = 74
                            translateHandler.selectLanguage(translations[index].id)
                            var tmp = config.config
                            tmp.language = translations[index].id
                            config.config = tmp
                            config.writeConfig();
                        }
                    }
                }
            }

            highlight: Rectangle { color: section.height == 300 ? colorHighlight1 : colorBackgroundTransparent ; radius: cornerRadius }
        }
    }
}
