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

#include <QObject>

#include "environment.h"

class FactoryReset : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool supported READ isSupported CONSTANT)
    Q_PROPERTY(QString getError READ getError CONSTANT)

 public:
    explicit FactoryReset(Environment* environment, QObject* parent = nullptr);

    bool    isSupported() const;
    QString getError() const { return m_error; }

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void performReset();

 signals:
    void factoryResetStarted();
    void factoryResetCompleted();

    /**
     * @brief This signal is emitted if a download failed.
     * @param errorMsg A human-readable description of the error
     */
    void factoryResetFailed(const QString& errorMsg);

 private:
    QString m_error;

    Environment* m_environment;
};
