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

#include "jsonfile.h"

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>

JsonFile::JsonFile(QObject *parent) :
    QObject(parent)
{
}

JsonFile::JsonFile(const QString &name, QObject *parent) :
    QObject(parent), m_file(name)
{
}

void JsonFile::setName(const QString &name)
{
    QString localName = name;
    if (m_file.fileName() != localName) {
        m_file.setFileName(localName);
        emit nameChanged(localName);
    }
}

QString JsonFile::fileName() const
{
    return QFileInfo(m_file).fileName();
}

bool JsonFile::rename(const QString &newName)
{
    bool success = m_file.rename(newName);
    if (success) {
        emit nameChanged(newName);
    }
    return success;
}

bool JsonFile::write(const QVariantMap &data)
{
    if (m_file.fileName().isEmpty()) {
        m_error = tr("empty name");
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromVariant(data);
    if (doc.isNull()) {
        return false;
    }
    if (doc.isEmpty()) {
        m_error = tr("empty data");
        return false;
    }
    QByteArray json = doc.toJson();
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        m_error = tr("cannot open file '%1' for writing: %2")
                .arg(m_file.fileName()).arg((m_file.errorString()));
        return false;
    }
    bool success = m_file.write(json) == json.size();
    m_file.close();
    return success;
}

QVariant JsonFile::read()
{
    if (m_file.fileName().isEmpty()) {
        m_error = tr("empty name");
        return QVariant();
    }
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = tr("cannot open file '%1' for reading: %2")
                .arg(m_file.fileName()).arg((m_file.errorString()));
        return QVariant();
    }
    QByteArray json = m_file.readAll();
    m_file.close();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        m_error = tr("invalid JSON file '%1' at offset %2")
                .arg(error.errorString()).arg(error.offset);
        return QVariant();
    }
    return doc.toVariant();
}
