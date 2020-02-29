/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Virtual Keyboard module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.VirtualKeyboard 2.3

KeyboardLayoutLoader {
    function createInputMethod() {
        return Qt.createQmlObject('import QtQuick 2.0; import QtQuick.VirtualKeyboard.Plugins 2.3; TCInputMethod {}', parent, "tcInputMethod")
    }
    sharedLayouts: ['symbols']
    sourceComponent: InputContext.inputEngine.inputMode === InputEngine.InputMode.Cangjie ? pageCangjie : pageZhuyin
    Component {
        id: pageCangjie
        KeyboardLayout {
            keyWeight: 160
            smallTextVisible: true
            KeyboardRow {
                Key {
                    key: Qt.Key_0
                    text: "0"
                }
                Key {
                    key: Qt.Key_1
                    text: "1"
                }
                Key {
                    key: Qt.Key_2
                    text: "2"
                }
                Key {
                    key: Qt.Key_3
                    text: "3"
                }
                Key {
                    key: Qt.Key_4
                    text: "4"
                }
                Key {
                    key: Qt.Key_5
                    text: "5"
                }
                Key {
                    key: Qt.Key_6
                    text: "6"
                }
                Key {
                    key: Qt.Key_7
                    text: "7"
                }
                Key {
                    key: Qt.Key_8
                    text: "8"
                }
                Key {
                    key: Qt.Key_9
                    text: "9"
                }
            }
            KeyboardRow {
                Key {
                    text: "\u624B"
                    alternativeKeys: "\u624Bq"
                }
                Key {
                    text: "\u7530"
                    alternativeKeys: "\u7530w"
                }
                Key {
                    text: "\u6C34"
                    alternativeKeys: "\u6C34e"
                }
                Key {
                    text: "\u53E3"
                    alternativeKeys: "\u53E3r"
                }
                Key {
                    text: "\u5EFF"
                    alternativeKeys: "\u5EFFt"
                }
                Key {
                    text: "\u535C"
                    alternativeKeys: "\u535Cy"
                }
                Key {
                    text: "\u5C71"
                    alternativeKeys: "\u5C71u"
                }
                Key {
                    text: "\u6208"
                    alternativeKeys: "\u6208i"
                }
                Key {
                    text: "\u4EBA"
                    alternativeKeys: "\u4EBAo"
                }
                Key {
                    text: "\u5FC3"
                    alternativeKeys: "\u5FC3p"
                }
            }
            KeyboardRow {
                Key {
                    text: "\u65E5"
                    alternativeKeys: "\u65E5a"
                }
                Key {
                    text: "\u5C38"
                    alternativeKeys: "\u5C38s"
                }
                Key {
                    text: "\u6728"
                    alternativeKeys: "\u6728d"
                }
                Key {
                    text: "\u706B"
                    alternativeKeys: "\u706Bf"
                }
                Key {
                    text: "\u571F"
                    alternativeKeys: "\u571Fg"
                }
                Key {
                    text: "\u7AF9"
                    alternativeKeys: "\u7AF9h"
                }
                Key {
                    text: "\u5341"
                    alternativeKeys: "\u5341j"
                }
                Key {
                    text: "\u5927"
                    alternativeKeys: "\u5927k"
                }
                Key {
                    text: "\u4E2D"
                    alternativeKeys: "\u4E2Dl"
                }
            }
            KeyboardRow {
                ShiftKey { weight: 200 }
                ModeKey {
                    id: simplifiedModeKey
                    key: Qt.Key_Mode_switch
                    displayText: "速成"
                    Component.onCompleted: updateBinding()
                    Connections {
                        target: InputContext.inputEngine
                        onInputMethodChanged: simplifiedModeKey.updateBinding()
                    }
                    function updateBinding() {
                        if (InputContext.inputEngine.inputMethod && InputContext.inputEngine.inputMethod.hasOwnProperty("simplified")) {
                            simplifiedModeKey.mode = InputContext.inputEngine.inputMethod.simplified
                            InputContext.inputEngine.inputMethod.simplified = Qt.binding(function() { return simplifiedModeKey.mode })
                        }
                    }
                }
                Key {
                    text: "\u91CD"
                    alternativeKeys: "\u91CDz"
                }
                Key {
                    text: "\u96E3"
                    alternativeKeys: "\u96E3x"
                }
                Key {
                    text: "\u91D1"
                    alternativeKeys: "\u91D1c"
                }
                Key {
                    text: "\u5973"
                    alternativeKeys: "\u5973v"
                }
                Key {
                    text: "\u6708"
                    alternativeKeys: "\u6708b"
                }
                Key {
                    text: "\u5F13"
                    alternativeKeys: "\u5F13n"
                }
                Key {
                    text: "\u4E00"
                    alternativeKeys: "\u4E00m"
                }
                BackspaceKey {}
            }
            KeyboardRow {
                ChangeLanguageKey { weight: 100 }
                InputModeKey {
                    visible: InputContext.inputEngine.inputModes.indexOf(InputEngine.InputMode.Zhuyin) !== -1
                }
                SymbolModeKey { weight: 100 }
                Key {
                    text: " "
                    displayText: " "
                    repeat: true
                    showPreview: false
                    key: Qt.Key_Space
                    weight: 300
                }
                EnterKey {}
            }
        }
    }
    Component {
        id: pageZhuyin
        KeyboardLayout {
            smallTextVisible: true
            KeyboardRow {
                Layout.preferredHeight: 3
                KeyboardColumn {
                    Layout.preferredWidth: bottomRow.width - hideKeyboardKey.width
                    KeyboardRow {
                        Key {
                            text: "\u3105"
                            alternativeKeys: "\u31051"
                        }
                        Key {
                            text: "\u3109"
                            alternativeKeys: "\u31092"
                        }
                        Key {
                            text: "\u02C7"
                            alternativeKeys: "\u02C73"
                        }
                        Key {
                            text: "\u02CB"
                            alternativeKeys: "4"
                            displayText: "`"
                        }
                        Key {
                            text: "\u3113"
                            alternativeKeys: "\u31135"
                        }
                        Key {
                            text: "\u02CA"
                            alternativeKeys: "6"
                            displayText: "´"
                        }
                        Key {
                            text: "\u02D9"
                            alternativeKeys: "\u02D97"
                        }
                        Key {
                            text: "\u311A"
                            alternativeKeys: "\u311A8"
                        }
                        Key {
                            text: "\u311E"
                            alternativeKeys: "\u311E9"
                        }
                        Key {
                            text: "\u3122"
                            alternativeKeys: "\u31220"
                        }
                    }
                    KeyboardRow {
                        Key {
                            text: "\u3106"
                            alternativeKeys: "\u3106q"
                        }
                        Key {
                            text: "\u310A"
                            alternativeKeys: "\u310Aw"
                        }
                        Key {
                            text: "\u310D"
                            alternativeKeys: "\u310De"
                        }
                        Key {
                            text: "\u3110"
                            alternativeKeys: "\u3110r"
                        }
                        Key {
                            text: "\u3114"
                            alternativeKeys: "\u3114t"
                        }
                        Key {
                            text: "\u3117"
                            alternativeKeys: "\u3117y"
                        }
                        Key {
                            text: "\u3127"
                            alternativeKeys: "\u3127u"
                        }
                        Key {
                            text: "\u311B"
                            alternativeKeys: "\u311Bi"
                        }
                        Key {
                            text: "\u311F"
                            alternativeKeys: "\u311Fo"
                        }
                        Key {
                            text: "\u3123"
                            alternativeKeys: "\u3123p"
                        }
                    }
                    KeyboardRow {
                        Key {
                            text: "\u3107"
                            alternativeKeys: "\u3107a"
                        }
                        Key {
                            text: "\u310B"
                            alternativeKeys: "\u310Bs"
                        }
                        Key {
                            text: "\u310E"
                            alternativeKeys: "\u310Ed"
                        }
                        Key {
                            text: "\u3111"
                            alternativeKeys: "\u3111f"
                        }
                        Key {
                            text: "\u3115"
                            alternativeKeys: "\u3115g"
                        }
                        Key {
                            text: "\u3118"
                            alternativeKeys: "\u3118h"
                        }
                        Key {
                            text: "\u3128"
                            alternativeKeys: "\u3128j"
                        }
                        Key {
                            text: "\u311C"
                            alternativeKeys: "\u311Ck"
                        }
                        Key {
                            text: "\u3120"
                            alternativeKeys: "\u3120l"
                        }
                        Key {
                            text: "\u3124"
                            alternativeKeys: "\u3124…"
                        }
                    }
                    KeyboardRow {
                        Key {
                            text: "\u3108"
                            alternativeKeys: "\u3108z"
                        }
                        Key {
                            text: "\u310C"
                            alternativeKeys: "\u310Cx"
                        }
                        Key {
                            text: "\u310F"
                            alternativeKeys: "\u310Fc"
                        }
                        Key {
                            text: "\u3112"
                            alternativeKeys: "\u3112v"
                        }
                        Key {
                            text: "\u3116"
                            alternativeKeys: "\u3116b"
                        }
                        Key {
                            text: "\u3119"
                            alternativeKeys: "\u3119n"
                        }
                        Key {
                            text: "\u3129"
                            alternativeKeys: "\u3129m"
                        }
                        Key {
                            text: "\u311D"
                            alternativeKeys: "、\u311D，"
                        }
                        Key {
                            text: "\u3121"
                            alternativeKeys: "。\u3121．"
                        }
                        Key {
                            text: "\u3125"
                            alternativeKeys: "；\u3125："
                        }
                    }
                }
                KeyboardColumn {
                    Layout.preferredWidth: 0.2
                    KeyboardRow {
                        BackspaceKey {}
                    }
                    KeyboardRow {
                        EnterKey {}
                    }
                    KeyboardRow {
                        ShiftKey { }
                    }
                }
            }
            KeyboardRow {
                Layout.preferredHeight: 1
                ChangeLanguageKey { weight: 100 }
                InputModeKey {
                    visible: InputContext.inputEngine.inputModes.indexOf(InputEngine.InputMode.Cangjie) !== -1
                }
                SymbolModeKey { weight: 100 }
                Key {
                    text: " "
                    displayText: " "
                    repeat: true
                    showPreview: false
                    key: Qt.Key_Space
                    weight: 300
                }
                EnterKey {}
            }
        }
    }
}
