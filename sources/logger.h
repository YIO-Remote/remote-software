/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2019 Christian Riedl <ric@rts.co.at>
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

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QSet>
#include <QStringList>
#include <QTextStream>

class Logger : public QObject {
    Q_OBJECT

 public:
    static Logger* instance();

    /**
     * @brief Sets the log level of the app.
     * @details The log level is mapped to logging rules on logging categories prefixed with `yio.`
     * @param logLevel DEBUG, INFO, WARN, CRIT, FATAL
     */
    void      setLogLevel(const QString& logLevel);
    void      setLogLevel(QtMsgType logLevel);
    QtMsgType logLevel() { return m_logLevel; }

    QtMsgType toMsgType(const QString& logLevel) const;

    /**
     * @brief Adds custom log filter rules to the default rules defined in QT_LOGGING_RULES.
     * @details The applied filter order is undefined! Dependent filters must be defined in the same set entry,
     * separated by a newline. The custom rules are added after the default rules.
     * Logging rules documentation: https://doc.qt.io/qt-5.12/qloggingcategory.html#logging-rules
     */
    void addLogRule(const QSet<QString>& rules);
    void removeLogRule(const QSet<QString>& rules);
    /**
     * @brief Clears all custom log rules
     */
    void clearLogRules();

    void setDefaultHandlerEnabled(bool value) { m_defHandlerEnabled = value; }
    bool isDefaultHandlerEnabled() { return m_defHandlerEnabled; }

    // -- File logging target

    /**
     * @brief Sets the log file directory, if empty no log files will be written.
     */
    void setLogDirectory(const QString& directory);
    /**
     * @brief Sets the number of hours after which log files will be removed. 0 = disabled.
     */
    void setLogFilePurgeHours(int hours) { m_purgeFileHours = hours; }

    /**
     * @brief Starts or stops the file logging target.
     * @details A new log file is automatically created when enabled, otherwise the log file will be closed.
     */
    void setFileEnabled(bool fileLogging);
    bool isFileEnabled() { return m_fileEnabled; }

    /**
     * @brief Returns the number of written log files
     */
    int getFileCount();

    /**
     * @brief Purges log files which are older than the given number of hours.
     * The current log file will never be purged.
     * @return The number of purged files.
     */
    int purgeFiles(int purgeHours);

    // -- Queue logging target for YIO API use. TODO(zehnm) still required?

    bool        isShowSourcePos() { return m_showSource; }
    void        setShowSourcePos(bool value) { m_showSource = value; }
    void        setQueueSize(int size);
    bool        isQueueEnabled() { return m_queueEnabled; }
    void        setQueueEnabled(bool value) { m_queueEnabled = value; }
    QJsonArray  getQueuedMessages(int maxCount, QtMsgType logLevel, const QStringList& categories);
    QJsonObject getInformation();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    /**
     * @brief Triggers file rotation
     */
    void rotateLogFile();

 signals:
    /**
     * @brief This signal is emitted if an error occured which should be displayed to the user.
     * @details Intended to connect to the notification handler to show a popup error to the user.
     * @param errorMsg A human-readable description of the error.
     */
    void userError(const QString& errorMsg);

    /**
     * @brief This signal is emitted for every log message which is not filtered by a logging rule.
     * @details Intended for a UI log viewer or to relay log messages to external system (e.g. web-configurator).
     * @param type The log type
     * @param logMsg The original log message.
     */
    void logViewerLog(QtMsgType type, const QString& logMsg);

 private:
    // singleton class, may not be instantiated
    explicit Logger(QObject* parent = nullptr);
    ~Logger();

    void close();

    // custom Qt log message handler
    static void yioMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    // main log message processor
    void processMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    void setLogRules(const QSet<QString>& rules);

    QStringList getExistingLogFileNames(const QDir& logDir);
    bool        createNewLogFile(const QString& fileName);

    void writeFile(const QString& message);

    struct SMessage {
        SMessage(QtMsgType type, uint timestamp, const QString& category, const QString& message,
                 const QString& sourcePosition)
            : type(type), timestamp(timestamp), category(category), message(message), sourcePosition(sourcePosition) {}
        QtMsgType type;
        uint      timestamp;  // unix time
        QString   category;
        QString   message;
        QString   sourcePosition;
    };

    void writeQueue(const SMessage& message);

 private:
    QtMsgType     m_logLevel;
    QSet<QString> m_logRules;
    bool          m_defHandlerEnabled;

    // file related logging
    bool                        m_fileEnabled;      // Output to log file
    int                         m_purgeFileHours;   // Log file expiration in hours
    int                         m_lastRotationDay;  // Day of the month of last file rotation
    QString                     m_logDirectory;     // Log file directory
    QFile                       m_logFile;          // Current log file
    QMutex                      m_mutex;            // Thread-safe file access
    QScopedPointer<QTextStream> m_logStream;        // Log file output stream

    // queue related logging
    bool             m_queueEnabled;  // output to queue for JSON API
    bool             m_showSource;    // Show source file and line in queue messages
    int              m_maxQueueSize;  // Maximum queue size
    QQueue<SMessage> m_queue;         // Queue
};
