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

#pragma once

#include <QFile>
#include <QObject>
#include <QTextStream>

class FileIO : public QObject {
    Q_OBJECT

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    bool write(const QString& source, const QString& data) {
        if (source.isEmpty()) return false;

        QFile file(source);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)) return false;

        QTextStream out(&file);
        out << data;
        file.close();
        return true;
    }

    QString read(const QString& source) {
        QString returnText;

        QFile file(source);
        if (!file.exists()) {
            return QString("");
        }

        if (file.open(QFile::ReadOnly)) {
            QTextStream stream(&file);
            returnText = stream.readAll();
        }

        return returnText;
    }

    bool exists(const QString& source) {
        QFile file(source);

        if (file.exists()) {
            return true;
        } else {
            return false;
        }
    }

 public:
    FileIO() {}
};
