/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.0
import QtQuick.VirtualKeyboard 2.3

KeyboardLayoutLoader {
    sharedLayouts: ['symbols']
    sourceComponent: InputContext.inputEngine.inputMode === InputEngine.InputMode.Greek ? greekLayout : latinLayout
    Component {
        id: greekLayout
        KeyboardLayout {
            keyWeight: 160
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
                    text: "ς"
                }
                Key {
                    text: "ε"
                    alternativeKeys: "εέ"
                }
                Key {
                    text: "ρ"
                }
                Key {
                    text: "τ"
                }
                Key {
                    text: "ψ"
                }
                Key {
                    text: "υ"
                    alternativeKeys: "υύϋΰ"
                }
                Key {
                    text: "θ"
                }
                Key {
                    text: "ι"
                    alternativeKeys: "ιίϊΐ"
                }
                Key {
                    text: "ο"
                    alternativeKeys: "οό"
                }
                Key {
                    text: "π"
                }
            }
            KeyboardRow {
                Key {
                    text: "α"
                    alternativeKeys: "αά"
                }
                Key {
                    text: "σ"
                }
                Key {
                    text: "δ"
                }
                Key {
                    text: "φ"
                }
                Key {
                    text: "γ"
                }
                Key {
                    text: "η"
                    alternativeKeys: "ηή"
                }
                Key {
                    text: "ξ"
                }
                Key {
                    text: "κ"
                }
                Key {
                    text: "λ"
                }
            }
            KeyboardRow {
                ShiftKey { weight:  200 }
                Key {
                    text: "ζ"
                }
                Key {
                    text: "χ"
                }
                Key {
                    text: "ψ"
                }
                Key {
                    text: "ω"
                    alternativeKeys: "ωώ"
                }
                Key {
                    text: "β"
                }
                Key {
                    text: "ν"
                }
                Key {
                    text: "μ"
                }
                BackspaceKey {}
            }
            KeyboardRow {
                ChangeLanguageKey { weight: 100 }
                InputModeKey {
                    weight: 100
                    inputModes: [InputEngine.InputMode.Greek, InputEngine.InputMode.Latin]
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
        id: latinLayout
        KeyboardLayout {
            keyWeight: 160
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
                    key: Qt.Key_Q
                    text: "q"
                }
                Key {
                    key: Qt.Key_W
                    text: "w"
                }
                Key {
                    key: Qt.Key_E
                    text: "e"
                    alternativeKeys: "êeëèé"
                }
                Key {
                    key: Qt.Key_R
                    text: "r"
                    alternativeKeys: "ŕrř"
                }
                Key {
                    key: Qt.Key_T
                    text: "t"
                    alternativeKeys: "ţtŧť"
                }
                Key {
                    key: Qt.Key_Y
                    text: "y"
                    alternativeKeys: "ÿyýŷ"
                }
                Key {
                    key: Qt.Key_U
                    text: "u"
                    alternativeKeys: "űūũûüuùú"
                }
                Key {
                    key: Qt.Key_I
                    text: "i"
                    alternativeKeys: "îïīĩiìí"
                }
                Key {
                    key: Qt.Key_O
                    text: "o"
                    alternativeKeys: "œøõôöòóo"
                }
                Key {
                    key: Qt.Key_P
                    text: "p"
                }
            }
            KeyboardRow {
                Key {
                    key: Qt.Key_A
                    text: "a"
                    alternativeKeys: (InputContext.inputMethodHints & (Qt.ImhEmailCharactersOnly | Qt.ImhUrlCharactersOnly)) ? "a@äåãâàá" : "aäåãâàá"
                    smallTextVisible: (InputContext.inputMethodHints & (Qt.ImhEmailCharactersOnly | Qt.ImhUrlCharactersOnly))
                }
                Key {
                    key: Qt.Key_S
                    text: "s"
                    alternativeKeys: "šsşś"
                }
                Key {
                    key: Qt.Key_D
                    text: "d"
                    alternativeKeys: "dđď"
                }
                Key {
                    key: Qt.Key_F
                    text: "f"
                }
                Key {
                    key: Qt.Key_G
                    text: "g"
                    alternativeKeys: "ġgģĝğ"
                }
                Key {
                    key: Qt.Key_H
                    text: "h"
                }
                Key {
                    key: Qt.Key_J
                    text: "j"
                }
                Key {
                    key: Qt.Key_K
                    text: "k"
                }
                Key {
                    key: Qt.Key_L
                    text: "l"
                    alternativeKeys: "ĺŀłļľl"
                }
            }
            KeyboardRow {
                ShiftKey { weight:  200 }
                Key {
                    key: Qt.Key_Z
                    text: "z"
                    alternativeKeys: "zžż"
                }
                Key {
                    key: Qt.Key_X
                    text: "x"
                }
                Key {
                    key: Qt.Key_C
                    text: "c"
                    alternativeKeys: "çcċčć"
                }
                Key {
                    key: Qt.Key_V
                    text: "v"
                }
                Key {
                    key: Qt.Key_B
                    text: "b"
                }
                Key {
                    key: Qt.Key_N
                    text: "n"
                    alternativeKeys: "ņńnň"
                }
                Key {
                    key: Qt.Key_M
                    text: "m"
                }
                Key {
                    key: Qt.Key_Comma
                    text: ","
                }
                Key {
                    key: Qt.Key_Period
                    text: "."
                }
                BackspaceKey {}
            }
            KeyboardRow {
                ChangeLanguageKey { weight: 100 }
                InputModeKey {
                    weight: 100
                    inputModes: [InputEngine.InputMode.Greek, InputEngine.InputMode.Latin]
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
