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

#ifndef JSONFILE_H
#define JSONFILE_H

#include <QObject>
#include <QFile>
#include <QVariant>

class JsonFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY nameChanged)
    Q_PROPERTY(bool exists READ exists)
    Q_PROPERTY(bool writeable READ writeable)
    Q_PROPERTY(bool readable READ readable)
    Q_PROPERTY(qint64 size READ size)
    Q_PROPERTY(QString error READ error)

public:
    explicit JsonFile(QObject *parent = 0);
    JsonFile(const QString &name, QObject *parent = 0);

    inline QString name() const { return m_file.fileName(); }
    QString fileName() const;
    inline bool exists() const { return m_file.exists(); }
    inline bool writeable() const { return m_file.permissions().testFlag(QFileDevice::WriteUser); }
    inline bool readable() const { return m_file.permissions().testFlag(QFileDevice::ReadUser); }
    inline qint64 size() const { return m_file.size(); }
    inline QString error() const { return m_error; }

//    Q_INVOKABLE QString relativeFilePath(const QString &dir = QString()) const;
    Q_INVOKABLE bool rename(const QString &newName);

    Q_INVOKABLE inline bool copy(const QString &newName) { return m_file.copy(newName); }
    Q_INVOKABLE inline bool remove() { return m_file.remove(); }

    Q_INVOKABLE bool write(const QVariantMap &data);
    Q_INVOKABLE QVariant read();

signals:
    void nameChanged(const QString &name);

public slots:
    void setName(const QString &name);

private:
    QFile m_file;
    QString m_error;
};

#endif // JSONFILE_H
