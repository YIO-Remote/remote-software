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

#ifndef JSONFILE_H_
#define JSONFILE_H_

#include <QObject>
#include <QFile>
#include <QVariant>

class JsonFile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString     name           READ name           WRITE setName           NOTIFY nameChanged)
    Q_PROPERTY(QString     fileName       READ fileName                               NOTIFY nameChanged)
    Q_PROPERTY(QString     schemaPath     READ schemaPath     WRITE setSchemaPath)
    Q_PROPERTY(bool        exists         READ exists)
    Q_PROPERTY(bool        writeable      READ writeable)
    Q_PROPERTY(bool        readable       READ readable)
    Q_PROPERTY(qint64      size           READ size)
    Q_PROPERTY(bool        valid          READ isValid                                CONSTANT)
    Q_PROPERTY(QString     error          READ error                                  CONSTANT)

 public:
    explicit JsonFile(QObject *parent = nullptr);
    JsonFile(const QString &path, const QString &schemaPath, QObject *parent = nullptr);

    /**
     * @brief name Returns the full file name including path
     */
    inline QString name() const { return m_file.fileName(); }
    /**
     * @brief fileName Returns the filename without path.
     */
    QString fileName() const;
    /**
     * @brief schemaName Returns the full JSON schema file name including path
     */
    inline QString schemaPath() const { return m_schemaPath; }
    inline void setSchemaPath(const QString &name) { m_schemaPath = name; }
    inline bool exists() const { return m_file.exists(); }
    inline bool writeable() const { return m_file.permissions().testFlag(QFileDevice::WriteUser); }
    inline bool readable() const { return m_file.permissions().testFlag(QFileDevice::ReadUser); }
    inline qint64 size() const { return m_file.size(); }
    bool isValid() const { return m_error.isEmpty(); }
    inline QString error() const { return m_error; }

    Q_INVOKABLE bool rename(const QString &newName);

    Q_INVOKABLE inline bool copy(const QString &newName) { return m_file.copy(newName); } //NOLINT false positive: doesn't like copy name
    Q_INVOKABLE inline bool remove() { return m_file.remove(); }

    Q_INVOKABLE bool write(const QVariantMap &data);
    Q_INVOKABLE QVariant read();

    /**
     * @brief validate Validates the JSON document against the associated schema of the JsonFile instance.
     * @param doc The JSON document to validate
     * @param errorText Returns the validation error text
     * @return true if the document is valid according to the schema
     */
    bool validate(const QJsonDocument &doc, QString &errorText); //NOLINT we do not want a pointer for errorText

    /**
     * @brief validate Validates the JSON document against the given JSON schema.
     * @param doc The JSON document to validate
     * @param schema The JSON schema document
     * @param errorText Returns the validation error text
     * @return true if the document is valid according to the schema
     */
    static bool validate(const QJsonDocument &doc, const QJsonDocument &schema, QString &errorText); //NOLINT we do not want a pointer for errorText

 signals:
    void nameChanged(const QString &name);

 public slots: //NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    /**
     * @brief setName Sets the full file name
     * @param name The file name including path
     */
    void setName(const QString &name);

 private:
    bool loadDocument(const QString &path, QJsonDocument &doc); //NOLINT we do not want a pointer for doc

    QFile   m_file;
    QString m_schemaPath;
    QString m_error;
};

#endif  // JSONFILE_H_
