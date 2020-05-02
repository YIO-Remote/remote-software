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

import QtQuick 2.10
import QtQuick.VirtualKeyboard 2.3

KeyboardLayoutLoader {
    sharedLayouts: ['symbols']
    sourceComponent: InputContext.inputEngine.inputMode === InputEngine.InputMode.Cyrillic ? cyrillicLayout : latinLayout
    Component {
        id: cyrillicLayout
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
                    key: 0x0419
                    text: "й"
                }
                Key {
                    key: 0x0426
                    text: "ц"
                }
                Key {
                    key: 0x0423
                    text: "у"
                }
                Key {
                    key: 0x041A
                    text: "к"
                }
                Key {
                    key: 0x0415
                    text: "е"
                    alternativeKeys: "её"
                }
                Key {
                    key: 0x041D
                    text: "н"
                }
                Key {
                    key: 0x0413
                    text: "г"
                }
                Key {
                    key: 0x0428
                    text: "ш"
                }
                Key {
                    key: 0x0429
                    text: "щ"
                }
                Key {
                    key: 0x0417
                    text: "з"
                }
                Key {
                    key: 0x0425
                    text: "х"
                }
            }
            KeyboardRow {
                Key {
                    key: 0x0424
                    text: "ф"
                }
                Key {
                    key: 0x042B
                    text: "ы"
                }
                Key {
                    key: 0x0412
                    text: "в"
                }
                Key {
                    key: 0x0410
                    text: "а"
                }
                Key {
                    key: 0x041F
                    text: "п"
                }
                Key {
                    key: 0x0420
                    text: "р"
                }
                Key {
                    key: 0x041E
                    text: "о"
                }
                Key {
                    key: 0x041B
                    text: "л"
                }
                Key {
                    key: 0x0414
                    text: "д"
                }
                Key {
                    key: 0x0416
                    text: "ж"
                }
                Key {
                    key: 0x042D
                    text: "э"
                }
            }
            KeyboardRow {
                ShiftKey { weight: 200 }
                Key {
                    key: 0x042F
                    text: "я"
                }
                Key {
                    key: 0x0427
                    text: "ч"
                }
                Key {
                    key: 0x0421
                    text: "с"
                }
                Key {
                    key: 0x041C
                    text: "м"
                }
                Key {
                    key: 0x0418
                    text: "и"
                }
                Key {
                    key: 0x0422
                    text: "т"
                }
                Key {
                    key: 0x042C
                    text: "ь"
                    alternativeKeys: "ьъ"
                }
                Key {
                    key: 0x0411
                    text: "б"
                }
                Key {
                    key: 0x042E
                    text: "ю"
                }
                BackspaceKey {}
            }
            KeyboardRow {
                ChangeLanguageKey { weight: 100 }
                InputModeKey {
                    weight: 100
                    inputModes: [InputEngine.InputMode.Cyrillic, InputEngine.InputMode.Latin]
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
                }
                Key {
                    key: Qt.Key_R
                    text: "r"
                }
                Key {
                    key: Qt.Key_T
                    text: "t"
                }
                Key {
                    key: Qt.Key_Z
                    text: "z"
                    alternativeKeys: "zž"
                }
                Key {
                    key: Qt.Key_U
                    text: "u"
                }
                Key {
                    key: Qt.Key_I
                    text: "i"
                }
                Key {
                    key: Qt.Key_O
                    text: "o"
                    alternativeKeys: "oö"
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
                    alternativeKeys: "aåä"
                }
                Key {
                    key: Qt.Key_S
                    text: "s"
                    alternativeKeys: "sš"
                }
                Key {
                    key: Qt.Key_D
                    text: "d"
                    alternativeKeys: "dđ"
                }
                Key {
                    key: Qt.Key_F
                    text: "f"
                }
                Key {
                    key: Qt.Key_G
                    text: "g"
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
                }
            }
            KeyboardRow {
                ShiftKey { weight: 200 }
                Key {
                    key: Qt.Key_Y
                    text: "y"
                }
                Key {
                    key: Qt.Key_X
                    text: "x"
                }
                Key {
                    key: Qt.Key_C
                    text: "c"
                    alternativeKeys: "ćcč"
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
                }
                Key {
                    key: Qt.Key_M
                    text: "m"
                }
                BackspaceKey {}
            }
            KeyboardRow {
                ChangeLanguageKey { weight: 100 }
                InputModeKey {
                    weight: 100
                    inputModes: [InputEngine.InputMode.Cyrillic, InputEngine.InputMode.Latin]
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
