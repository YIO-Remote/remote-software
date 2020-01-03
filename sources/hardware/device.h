/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#pragma once

#include <QIODevice>
#include <QObject>

/**
 * @brief The Device class is the base interface class of all YIO hardware devices.
 */
class Device : public QObject {
    Q_OBJECT

 public:
    explicit Device(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Device();

    /**
     * @brief isOpen Returns \c true if the device is open; otherwise returns \c false
     */
    bool isOpen() const;

    /**
     * @brief open Opens the device and sets its open flag to true and resets the error string.
     * @details This function should be called from any reimplementations of open().
     * @return Returns \c true if successful; otherwise returns \c false.
     */
    virtual bool open();

    /**
     * @brief close First emits aboutToClose(), then closes the device and sets its open flag to false.
     * @details This function should be called from any reimplementations of close().
     */
    virtual void close();

    /**
     * @brief errorString Returns a human-readable description of the last device error that occurred.
     */
    QString errorString() const;

    /**
     * @brief setErrorString Sets the human readable description of the last device error that occurred to \a str.
     * @param str
     */
    void setErrorString(const QString &str);

 signals:
    /**
     * @brief aboutToClose This signal is emitted when the device is about to close.
     * @details Connect this signal if you have operations that need to be performed before the device closes.
     */
    void aboutToClose();

 private:
    Q_DISABLE_COPY(Device)

    bool    m_open;
    QString m_errorString;
};
