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

#include "../logging.h"

// debug macro from https://stackoverflow.com/a/1644898
#ifdef QT_DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

// ASSERT_DEVICE_OPEN is a safety check if the device is really open.
// It should only be used where it is expected that the device is open and
// MAY NOT be used where it's possible that the device could be closed!
#define ASSERT_DEVICE_OPEN(retVal) \
    do { \
      if (DEBUG_TEST) { \
        if (!isOpen()) { \
            qCWarning(logCategory()) << DBG_WARN_DEVICE_CLOSED; \
            return retVal; \
        } \
      } \
    } while (0);
// end debug macro

/**
 * @brief The Device class is the base interface class of all YIO hardware devices.
 */
class Device : public QObject {
    Q_OBJECT

 public:
    enum DeviceError {
        None,
        NotFound,
        InitializationError,
        CommunicationError
    };
    Q_ENUM(DeviceError)

    /**
     * @brief Device
     * @param name A human readable name of the device. Intended for debug messages.
     * @param parent
     */
    explicit Device(QString name, QObject *parent = nullptr);
    virtual ~Device();

    /**
     * @brief name Returns a human readable name of the device. Intended for debug messages.
     */
    const QString &name() const { return m_name; }

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
     * @brief ready This signal is emitted when the device was successfully initialized and ready to use.
     */
    void ready();

    /**
     * @brief aboutToClose This signal is emitted when the device is about to close.
     * @details Connect this signal if you have operations that need to be performed before the device closes.
     */
    void aboutToClose();

    void error(DeviceError deviceError, const QString &message);

 protected:
    virtual const QLoggingCategory &logCategory() const;

    // reduce copy paste of common error string. I'm also not sure if the compiler would optimize duplicated strings...
    static constexpr const char* DBG_DEVICE_OPENED = "Successfully opened";
    static constexpr const char* DBG_WARN_DEVICE_OPEN  = "Device is already open. Ignoring open() call!";
    static constexpr const char* DBG_WARN_DEVICE_CLOSED = "Device is closed. Ignoring call!";

 private:
    Q_DISABLE_COPY(Device)

    bool    m_open;
    QString m_name;
    QString m_errorString;
};
