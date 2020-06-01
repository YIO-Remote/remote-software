/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
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

#include "buttonhandler.h"

#include <QLoggingCategory>
#include <QtDebug>

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.buttonhandler");

ButtonHandler *ButtonHandler::s_instance = nullptr;

ButtonHandler::ButtonHandler(InterruptHandler *interruptHandler, YioAPI *api, QObject *parent)
    : QObject(parent), m_itnerruptHandler(interruptHandler), m_api(api) {
    s_instance = this;

    // connect to interrupt handler
    connect(m_itnerruptHandler, &InterruptHandler::interruptEvent, this, &ButtonHandler::onInterrupt);

    // connect to API
    connect(m_api, &YioAPI::buttonPressed, this, &ButtonHandler::onYIOAPIPressed);
    connect(m_api, &YioAPI::buttonReleased, this, &ButtonHandler::onYIOAPIReleased);

    qCDebug(CLASS_LC) << "Intialized";
}

ButtonHandler::~ButtonHandler() { s_instance = nullptr; }

QObject *ButtonHandler::getQMLInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance;
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

void ButtonHandler::onInterrupt(int event) {
    switch (event) {
        case (DPAD_UP): {
            emit buttonPressed(DPAD_UP);
        } break;
        case (DPAD_UP_RELEASED): {
            emit buttonReleased(DPAD_UP);
        } break;
        case (DPAD_DOWN): {
            emit buttonPressed(DPAD_DOWN);
        } break;
        case (DPAD_DOWN_RELEASED): {
            emit buttonReleased(DPAD_DOWN);
        } break;
        case (DPAD_LEFT): {
            emit buttonPressed(DPAD_LEFT);
        } break;
        case (DPAD_LEFT_RELEASED): {
            emit buttonReleased(DPAD_LEFT);
        } break;
        case (DPAD_RIGHT): {
            emit buttonPressed(DPAD_RIGHT);
        } break;
        case (DPAD_RIGHT_RELEASED): {
            emit buttonReleased(DPAD_RIGHT);
        } break;
        case (DPAD_MIDDLE): {
            emit buttonPressed(DPAD_MIDDLE);
        } break;
        case (DPAD_MIDDLE_RELEASED): {
            emit buttonReleased(DPAD_MIDDLE);
        } break;
        case (TOP_LEFT): {
            emit buttonPressed(TOP_LEFT);
        } break;
        case (TOP_LEFT_RELEASED): {
            emit buttonReleased(TOP_LEFT);
        } break;
        case (TOP_RIGHT): {
            emit buttonPressed(TOP_RIGHT);
        } break;
        case (TOP_RIGHT_RELEASED): {
            emit buttonReleased(TOP_RIGHT);
        } break;
        case (BOTTOM_LEFT): {
            emit buttonPressed(BOTTOM_LEFT);
        } break;
        case (BOTTOM_LEFT_RELEASED): {
            emit buttonReleased(BOTTOM_LEFT);
        } break;
        case (BOTTOM_RIGHT): {
            emit buttonPressed(BOTTOM_RIGHT);
        } break;
        case (BOTTOM_RIGHT_RELEASED): {
            emit buttonReleased(BOTTOM_RIGHT);
        } break;
        case (VOLUME_UP): {
            emit buttonPressed(VOLUME_UP);
        } break;
        case (VOLUME_UP_RELEASED): {
            emit buttonReleased(VOLUME_UP);
        } break;
        case (VOLUME_DOWN): {
            emit buttonPressed(VOLUME_DOWN);
        } break;
        case (VOLUME_DOWN_RELEASED): {
            emit buttonReleased(VOLUME_DOWN);
        } break;
        case (CHANNEL_UP): {
            emit buttonPressed(CHANNEL_UP);
        } break;
        case (CHANNEL_UP_RELEASED): {
            emit buttonReleased(CHANNEL_UP);
        } break;
        case (CHANNEL_DOWN): {
            emit buttonPressed(CHANNEL_DOWN);
        } break;
        case (CHANNEL_DOWN_RELEASED): {
            emit buttonReleased(CHANNEL_DOWN);
        } break;
    }
}

void ButtonHandler::onYIOAPIPressed(QString button) {
    if (button == "dpad up") {
        emit buttonPressed(DPAD_UP);
    }
    if (button == "dpad down") {
        emit buttonPressed(DPAD_DOWN);
    }
    if (button == "dpad left") {
        emit buttonPressed(DPAD_LEFT);
    }
    if (button == "dpad right") {
        emit buttonPressed(DPAD_RIGHT);
    }
    if (button == "dpad middle") {
        emit buttonPressed(DPAD_MIDDLE);
    }
    if (button == "top left") {
        emit buttonPressed(TOP_LEFT);
    }
    if (button == "top right") {
        emit buttonPressed(TOP_RIGHT);
    }
    if (button == "bottom left") {
        emit buttonPressed(BOTTOM_LEFT);
    }
    if (button == "bottom right") {
        emit buttonPressed(BOTTOM_RIGHT);
    }
    if (button == "volume up") {
        emit buttonPressed(VOLUME_UP);
    }
    if (button == "volume down") {
        emit buttonPressed(VOLUME_DOWN);
    }
    if (button == "channel up") {
        emit buttonPressed(CHANNEL_UP);
    }
    if (button == "channel down") {
        emit buttonPressed(CHANNEL_DOWN);
    }
}

void ButtonHandler::onYIOAPIReleased(QString button) {
    if (button == "dpad up") {
        emit buttonReleased(DPAD_UP);
    }
    if (button == "dpad down") {
        emit buttonReleased(DPAD_DOWN);
    }
    if (button == "dpad left") {
        emit buttonReleased(DPAD_LEFT);
    }
    if (button == "dpad right") {
        emit buttonReleased(DPAD_RIGHT);
    }
    if (button == "dpad middle") {
        emit buttonReleased(DPAD_MIDDLE);
    }
    if (button == "top left") {
        emit buttonReleased(TOP_LEFT);
    }
    if (button == "top right") {
        emit buttonReleased(TOP_RIGHT);
    }
    if (button == "bottom left") {
        emit buttonReleased(BOTTOM_LEFT);
    }
    if (button == "bottom right") {
        emit buttonReleased(BOTTOM_RIGHT);
    }
    if (button == "volume up") {
        emit buttonReleased(VOLUME_UP);
    }
    if (button == "volume down") {
        emit buttonReleased(VOLUME_DOWN);
    }
    if (button == "channel up") {
        emit buttonReleased(CHANNEL_UP);
    }
    if (button == "channel down") {
        emit buttonReleased(CHANNEL_DOWN);
    }
}
