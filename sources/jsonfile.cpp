/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
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

#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/adapters/qtjson_adapter.hpp>
#include <valijson/utils/qtjson_utils.hpp>

#include <iostream>

#include <QLoggingCategory>
#include <QtDebug>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>

#include "jsonfile.h"


using std::endl;

using valijson::Schema;
using valijson::SchemaParser;
using valijson::Validator;
using valijson::ValidationResults;
using valijson::adapters::QtJsonAdapter;

static Q_LOGGING_CATEGORY(CLASS_LC, "json");

JsonFile::JsonFile(QObject *parent) :
    QObject(parent)
{
}

JsonFile::JsonFile(const QString &path, const QString &schemaPath, QObject *parent)
    : QObject(parent)
    , m_file(path)
    , m_schemaPath(schemaPath)
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
    m_error.clear();
    if (m_file.fileName().isEmpty()) {
        qCWarning(CLASS_LC) << "Not writing json file: no filename set!";
        m_error = tr("empty name");
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromVariant(data);
    if (doc.isNull() || doc.isEmpty()) {
        qCWarning(CLASS_LC) << "Not writing json file" << m_file.fileName() << ": empty data!";
        m_error = tr("empty data");
        return false;
    }

    if (!validate(doc, m_error)) {
        qCWarning(CLASS_LC) << "JSON document failed schema validation before writing:" << m_file.fileName();
        return false;
    }

    QByteArray json = doc.toJson();
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        m_error = tr("cannot open file '%1' for writing: %2")
                .arg(m_file.fileName()).arg((m_file.errorString()));
        qCWarning(CLASS_LC) << m_error;
        return false;
    }
    bool success = m_file.write(json) == json.size();
    m_file.close();
    return success;
}

QVariant JsonFile::read()
{
    m_error.clear();
    QJsonDocument doc;
    if (!loadDocument(m_file.fileName(), doc)) {
        return QVariant();
    }

    if (!validate(doc, m_error)) {
        qCWarning(CLASS_LC) << "Read JSON document failed schema validation:" << m_file.fileName();
        return QVariant();
    }

    return doc.toVariant();
}

bool JsonFile::loadDocument(const QString &path, QJsonDocument &doc)
{
    if (path.isEmpty()) {
        qCWarning(CLASS_LC) << "Cannot load json file: no filename set!";
        m_error = tr("empty name");
        return false;
    }

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = tr("cannot open file '%1' for reading: %2")
                .arg(file.fileName()).arg((file.errorString()));
        qCWarning(CLASS_LC) << m_error;
        return false;
    }
    QByteArray json = file.readAll();
    file.close();
    QJsonParseError error;
    doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        m_error = tr("invalid JSON file '%1' at offset %2")
                .arg(error.errorString()).arg(error.offset);
        qCCritical(CLASS_LC) << m_error;
        return false;
    }

    return true;
}

bool JsonFile::validate(const QJsonDocument &doc, QString &errorText)
{
    if (m_schemaPath.isEmpty()) {
        qCDebug(CLASS_LC) << "Skipping json document schema validation: no schema file set";
        return true;
    }

    // TODO in case there are too many read or write calls we should probably cache the schema document
     QJsonDocument schemaDoc;
    if (!loadDocument(m_schemaPath, schemaDoc)) {
        return false;
    }

    return JsonFile::validate(doc, schemaDoc, errorText);
}

bool JsonFile::validate(const QJsonDocument &doc, const QJsonDocument &schemaDoc, QString &errorText)
{
    // Parse JSON schema content using valijson
    Schema schema;
    SchemaParser parser;
    QtJsonAdapter schemaAdapter(schemaDoc.object());
    parser.populateSchema(schemaAdapter, schema);

    // Perform validation
    Validator validator;
    ValidationResults results;
    QtJsonAdapter targetDocumentAdapter(doc.isObject() ? QJsonValue(doc.object())
                                                       : doc.isArray() ? QJsonValue(doc.array())
                                                                       : QJsonValue());
    if (!validator.validate(schema, targetDocumentAdapter, &results)) {
        std::ostringstream err;
        err << "Validation failed." << endl;
        ValidationResults::Error error;
        unsigned int errorNum = 1;
        while (results.popError(error)) {
            err << "Error #" << errorNum << std::endl;
            err << "  ";
            for (const std::string &contextElement : error.context) {
                err << contextElement << " ";
            }
            err << endl;
            err << "    - " << error.description << endl;
            ++errorNum;
        }
        errorText = QString::fromStdString(err.str());
        return false;
    }

    return true;
}
