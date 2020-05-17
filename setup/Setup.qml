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

import QtQuick 2.11
import QtQuick.Controls 2.5
import Style 1.0


Item {
    width: 480; height: 800

    property string wifiSsid
    property string wifiPassword

    SwipeView {
        id: swipeView
        width: 480; height: 800
        interactive: false
        currentIndex: 0

        property bool wifiSuccess: false
        property bool dockSuccess: false
        property bool dockSkip: false

        SetupStep1 {}
        SetupStep2 {}
        SetupStepCountry {}
        SetupStep3 {}
        SetupStep5 { _swipeView: swipeView; _currentItem: SwipeView.isCurrentItem }
        Loader { source: swipeView.wifiSuccess ? "qrc:/setup/SetupStep6Success.qml" : "qrc:/setup/SetupStep6Failure.qml" }
        SetupStep7 {}
        SetupStep8 { _swipeView: swipeView; _currentItem: SwipeView.isCurrentItem }
        Loader { source: swipeView.dockSkip ? "qrc:/setup/SetupStep9Skip.qml" : (swipeView.dockSuccess ? "qrc:/setup/SetupStep9Success.qml" : "qrc:/setup/SetupStep9Failure.qml") }
    }

    Component.onCompleted:  {
        loadingScreen.item.state = "loaded";
    }
}
