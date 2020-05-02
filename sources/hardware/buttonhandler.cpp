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
            if (m_buttonPressed != DPAD_UP) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = DPAD_UP;
                emit buttonPressed(DPAD_UP);
                qCDebug(CLASS_LC) << "Dpad up pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = DPAD_UP;
                emit buttonReleased(DPAD_UP);
                qCDebug(CLASS_LC) << "Dpad up released";
            }
        } break;
        case (DPAD_DOWN): {
            if (m_buttonPressed != DPAD_DOWN) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = DPAD_DOWN;
                emit buttonPressed(DPAD_DOWN);
                qCDebug(CLASS_LC) << "Dpad down pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = DPAD_DOWN;
                emit buttonReleased(DPAD_DOWN);
                qCDebug(CLASS_LC) << "Dpad down released";
            }
        } break;
        case (DPAD_LEFT): {
            if (m_buttonPressed != DPAD_LEFT) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = DPAD_LEFT;
                emit buttonPressed(DPAD_LEFT);
                qCDebug(CLASS_LC) << "Dpad left pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = DPAD_LEFT;
                emit buttonReleased(DPAD_LEFT);
                qCDebug(CLASS_LC) << "Dpad left released";
            }
        } break;
        case (DPAD_RIGHT): {
            if (m_buttonPressed != DPAD_RIGHT) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = DPAD_RIGHT;
                emit buttonPressed(DPAD_RIGHT);
                qCDebug(CLASS_LC) << "Dpad right pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = DPAD_RIGHT;
                emit buttonReleased(DPAD_RIGHT);
                qCDebug(CLASS_LC) << "Dpad right released";
            }
        } break;
        case (DPAD_MIDDLE): {
            if (m_buttonPressed != DPAD_MIDDLE) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = DPAD_MIDDLE;
                emit buttonPressed(DPAD_MIDDLE);
                qCDebug(CLASS_LC) << "Dpad middle pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = DPAD_MIDDLE;
                emit buttonReleased(DPAD_MIDDLE);
                qCDebug(CLASS_LC) << "Dpad middle released";
            }
        } break;
        case (TOP_LEFT): {
            if (m_buttonPressed != TOP_LEFT) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = TOP_LEFT;
                emit buttonPressed(TOP_LEFT);
                qCDebug(CLASS_LC) << "Top left pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = TOP_LEFT;
                emit buttonReleased(TOP_LEFT);
                qCDebug(CLASS_LC) << "Top left released";
            }
        } break;
        case (TOP_RIGHT): {
            if (m_buttonPressed != TOP_RIGHT) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = TOP_RIGHT;
                emit buttonPressed(TOP_RIGHT);
                qCDebug(CLASS_LC) << "Top right pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = TOP_RIGHT;
                emit buttonReleased(TOP_RIGHT);
                qCDebug(CLASS_LC) << "Top right released";
            }
        } break;
        case (BOTTOM_LEFT): {
            if (m_buttonPressed != BOTTOM_LEFT) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = BOTTOM_LEFT;
                emit buttonPressed(BOTTOM_LEFT);
                qCDebug(CLASS_LC) << "Bottom left pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = BOTTOM_LEFT;
                emit buttonReleased(BOTTOM_LEFT);
                qCDebug(CLASS_LC) << "Bottom left released";
            }
        } break;
        case (BOTTOM_RIGHT): {
            if (m_buttonPressed != BOTTOM_RIGHT) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = BOTTOM_RIGHT;
                emit buttonPressed(BOTTOM_RIGHT);
                qCDebug(CLASS_LC) << "Bottom right pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = BOTTOM_RIGHT;
                emit buttonReleased(BOTTOM_RIGHT);
                qCDebug(CLASS_LC) << "Bottom right released";
            }
        } break;
        case (VOLUME_UP): {
            if (m_buttonPressed != VOLUME_UP) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = VOLUME_UP;
                emit buttonPressed(VOLUME_UP);
                qCDebug(CLASS_LC) << "Volume up pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = VOLUME_UP;
                emit buttonReleased(VOLUME_UP);
                qCDebug(CLASS_LC) << "Volume up released";
            }
        } break;
        case (VOLUME_DOWN): {
            if (m_buttonPressed != VOLUME_DOWN) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = VOLUME_DOWN;
                emit buttonPressed(VOLUME_DOWN);
                qCDebug(CLASS_LC) << "Volume down pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = VOLUME_DOWN;
                emit buttonReleased(VOLUME_DOWN);
                qCDebug(CLASS_LC) << "Volume down released";
            }
        } break;
        case (CHANNEL_UP): {
            if (m_buttonPressed != CHANNEL_UP) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = CHANNEL_UP;
                emit buttonPressed(CHANNEL_UP);
                qCDebug(CLASS_LC) << "Channel up pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = CHANNEL_UP;
                emit buttonReleased(CHANNEL_UP);
                qCDebug(CLASS_LC) << "Channel up released";
            }
        } break;
        case (CHANNEL_DOWN): {
            if (m_buttonPressed != CHANNEL_DOWN) {
                m_buttonReleased = -1;
                emit buttonReleased(-1);

                m_buttonPressed = CHANNEL_DOWN;
                emit buttonPressed(CHANNEL_DOWN);
                qCDebug(CLASS_LC) << "Channel down pressed";
            } else {
                m_buttonPressed = -1;

                m_buttonReleased = CHANNEL_DOWN;
                emit buttonReleased(CHANNEL_DOWN);
                qCDebug(CLASS_LC) << "Channel down released";
            }
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
