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

#include "logger.h"

#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>
#include <iostream>

#include "logging.h"

// Get the default Qt message handler.
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

Logger* Logger::instance() {
    static Logger log;
    return &log;
}

Logger::Logger(QObject* parent)
    : QObject(parent),
      m_logLevel(QtDebugMsg),
      m_defHandlerEnabled(true),
      m_fileEnabled(false),
      m_purgeFileHours(5 * 24),
      m_lastRotationDay(0),
      m_queueEnabled(false),
      m_showSource(true),
      m_maxQueueSize(0) {
    // set default message pattern if QT_MESSAGE_PATTERN is not set
    qSetMessagePattern(QStringLiteral(
        "%{time yyyy-MM-dd hh:mm:ss:zzz t} %{if-debug}DEBUG%{endif}%{if-info}INFO %{endif}%{if-warning}WARN "
        "%{endif}%{if-critical}CRIT %{endif}%{if-fatal}FATAL%{endif} [%{category}]:\t%{message}\t[%{file}:%{line}]"));

    qInstallMessageHandler(&yioMessageHandler);
}

Logger::~Logger() {
    qInstallMessageHandler(0);
    close();
}

void Logger::close() {
    QMutexLocker locker(&m_mutex);

    if (m_logStream) {
        m_logStream->flush();
        m_logStream.reset(nullptr);
        m_logFile.close();
    }
}

void Logger::yioMessageHandler(::QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    Logger::instance()->processMessage(type, context, msg);
}

void Logger::setLogLevel(const QString& logLevel) {
    setLogLevel(toMsgType(logLevel));
}

void Logger::setLogLevel(QtMsgType logLevel) {
    const QSet<QString> noDebugRules = {QStringLiteral("yio*.debug=false"), QStringLiteral("qml.debug=false")};
    const QSet<QString> noInfoRules = {QStringLiteral("yio*.info=false"), QStringLiteral("qml.info=false")};
    const QSet<QString> noWarningRules = {QStringLiteral("yio*.warning=false"), QStringLiteral("qml.warning=false")};
    const QSet<QString> noCriticalRules = {QStringLiteral("yio*.critical=false"), QStringLiteral("qml.critical=false")};

    m_logLevel = logLevel;

    switch (logLevel) {
        case QtInfoMsg:
            removeLogRule(noInfoRules + noWarningRules + noCriticalRules);
            addLogRule(noDebugRules);
            break;
        case QtWarningMsg:
            removeLogRule(noWarningRules + noCriticalRules);
            addLogRule(noDebugRules + noInfoRules);
            break;
        case QtCriticalMsg:
            removeLogRule(noCriticalRules);
            addLogRule(noDebugRules + noInfoRules + noWarningRules);
            break;
        case QtFatalMsg:
            addLogRule(noDebugRules + noInfoRules + noWarningRules + noCriticalRules);
            break;
        default:
            removeLogRule(noDebugRules + noInfoRules + noWarningRules + noCriticalRules);
    }
}

QtMsgType Logger::toMsgType(const QString& logLevel) const {
    QString level = logLevel.toUpper();

    if (level == QLatin1String("DEBUG")) {
        return QtMsgType::QtDebugMsg;
    } else if (level == QLatin1String("INFO")) {
        return QtMsgType::QtInfoMsg;
    } else if (level == QLatin1String("WARN") || level == QLatin1String("WARNING")) {
        return QtMsgType::QtWarningMsg;
    } else if (level == QLatin1String("CRIT") || level == QLatin1String("CRITICAL")) {
        return QtMsgType::QtCriticalMsg;
    } else if (level == QLatin1String("FATAL")) {
        return QtMsgType::QtFatalMsg;
    } else {
        return QtMsgType::QtInfoMsg;
    }
}

void Logger::addLogRule(const QSet<QString>& rules) {
    setLogRules(m_logRules + rules);
}

void Logger::removeLogRule(const QSet<QString>& rules) {
    setLogRules(m_logRules - rules);
}

void Logger::clearLogRules() {
    setLogRules({});
}

void Logger::setLogRules(const QSet<QString>& rules) {
    // Get default rules as baseline and append our custom rules. Last one wins!
    // Note: individual rules in QT_LOGGING_RULES can be separated by semicolon or newline
    static const QString defaultRules =
        qEnvironmentVariable("QT_LOGGING_RULES").replace(QLatin1Char(';'), QLatin1Char('\n'));

    m_logRules = rules;
    QLoggingCategory::setFilterRules(defaultRules + QLatin1Char('\n') + rules.values().join(QLatin1Char('\n')));
}

void Logger::processMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    if (m_defHandlerEnabled) {
        // forward to default Qt message handler to output message to console or journald on YIO remote
        (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
    }

    if (m_fileEnabled) {
        if (m_lastRotationDay != QDate::currentDate().day()) {
            rotateLogFile();
        }
        writeFile(qFormatLogMessage(type, context, msg));
    }

    if (m_queueEnabled) {
        QString sourcePosition;
        if (m_showSource && context.file != nullptr) {
            sourcePosition = QString("%1:%2").arg(context.file).arg(context.line);
        }
        QDateTime dt = QDateTime::currentDateTimeUtc();
        SMessage  message(type, dt.toTime_t(), context.category, msg, sourcePosition);
        writeQueue(message);
    }

    emit logViewerLog(type, msg);
}

// --- File target ------------------------------------------------------------

void Logger::setLogDirectory(const QString& directory) {
    m_logDirectory = directory;
    m_fileEnabled = !directory.isEmpty();

    if (m_fileEnabled) {
        rotateLogFile();
    } else {
        close();
    }
}

void Logger::setFileEnabled(bool fileLogging) {
    if (m_fileEnabled == fileLogging) {
        return;
    }

    if (fileLogging && m_logDirectory.isEmpty()) {
        qCWarning(lcLogger) << "Cannot enable file logging without a log directory. Call setLogDirectory first!";
        return;
    }

    m_fileEnabled = fileLogging;

    if (m_fileEnabled) {
        rotateLogFile();
    } else {
        close();
    }
}

void Logger::rotateLogFile() {
    m_lastRotationDay = QDate::currentDate().day();

    // logfiles are disabled if no log directory is set
    if (m_logDirectory.isEmpty()) {
        return;
    }

    // make sure log directory exists
    QDir dir(m_logDirectory);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));  // error handling in createNewLogFile if new log file cannot be created
    }

    // purge expired log files
    purgeFiles(m_purgeFileHours);

    // set new log file base name
    QDateTime now = QDateTime::currentDateTime();
    QString   newLogFileName = QString("yio-%1.log").arg(now.toString(QStringLiteral("yyyy-MM-dd")));

    // log file search pattern to retrieve file number of current day
    QRegExp regex(QStringLiteral(R"(yio-.*\.log\.(\d+))"));
    int     fileCountSameDay = -1;

    // Scan existing log files in log dir and determine current file number of the current day
    for (const QString& file : getExistingLogFileNames(dir)) {
        if (file.startsWith(newLogFileName) && regex.exactMatch(file)) {
            fileCountSameDay = qMax(fileCountSameDay, regex.cap(1).toInt());
        }
    }

    newLogFileName.append(QLatin1Char('.') + QString::number(fileCountSameDay + 1));

    createNewLogFile(dir.filePath(newLogFileName));
}

int Logger::getFileCount() {
    return getExistingLogFileNames(m_logDirectory).length();
}

int Logger::purgeFiles(int purgeHours) {
    if (purgeHours <= 0) {
        return 0;
    }

    QDir                 dir(m_logDirectory);
    QString              currentLogFilePath = m_logFile.fileName();
    QDateTime            now = QDateTime::currentDateTime();
    int                  purgedFileCount = 0;
    std::chrono::seconds expireSeconds(purgeHours * 60 * 60);

    for (const QString& file : getExistingLogFileNames(dir)) {
        QString filePath = dir.absoluteFilePath(file);
        // only delete files if enabled and protect current log file
        if (m_purgeFileHours > 0 && currentLogFilePath != filePath) {
            QFileInfo fileInfo(filePath);
            if (fileInfo.lastModified().addSecs(expireSeconds.count()) < now) {
                if (dir.remove(file)) {
                    purgedFileCount++;
                }
            }
        }
    }

    if (purgedFileCount > 0) {
        qCInfo(lcLogger) << "Log files purged:" << purgedFileCount;
    }

    return purgedFileCount;
}

QStringList Logger::getExistingLogFileNames(const QDir& logDir) {
    return logDir.entryList(QStringList(QStringLiteral("yio-*.log.*")), QDir::Files, QDir::Name);
}

bool Logger::createNewLogFile(const QString& fileName) {
    QMutexLocker locker(&m_mutex);
    if (m_logStream) {
        m_logStream.reset(nullptr);
        m_logFile.close();
    }

    if (fileName.isEmpty()) {
        return false;
    }

    m_logFile.setFileName(fileName);

    if (!m_logFile.open(QIODevice::WriteOnly)) {
        locker.unlock();  // avoid deadlock in case signal handler calls a log function
        emit userError(QString(tr("Error creating log file '%1'.")).arg(fileName));
        return false;
    }

    m_logStream.reset(new QTextStream(&m_logFile));
    return true;
}

void Logger::writeFile(const QString& message) {
    QMutexLocker lock(&m_mutex);

    if (m_logStream) {
        (*m_logStream) << message << endl;
        m_logStream->flush();
    }
}

// --- Queue target -----------------------------------------------------------

void Logger::setQueueSize(int size) {
    m_maxQueueSize = size;
    setQueueEnabled(size > 0);
}

void Logger::writeQueue(const SMessage& message) {
    static QMutex queueMutex;

    QMutexLocker lock(&queueMutex);
    if (m_queue.count() >= m_maxQueueSize) {
        m_queue.dequeue();
    }
    m_queue.enqueue(message);
}

QJsonArray Logger::getQueuedMessages(int maxCount, QtMsgType logLevel, const QStringList& categories) {
    QJsonArray array;
    int        i = 0;
    while (i < maxCount && !m_queue.isEmpty()) {
        SMessage msg = m_queue.dequeue();
        switch (logLevel) {
            case QtFatalMsg:
                continue;
            case QtCriticalMsg:
                if (msg.type != QtCriticalMsg) {
                    continue;
                }
                break;
            case QtWarningMsg:
                if (msg.type == QtDebugMsg || msg.type == QtInfoMsg) {
                    continue;
                }
                break;
            case QtInfoMsg:
                if (msg.type == QtDebugMsg) {
                    continue;
                }
                break;
            default: {
            }  // all log messages
        }

        if (categories.length() > 0 && !categories.contains(msg.category)) {
            continue;
        }
        QJsonObject obj;
        obj.insert("type", msg.type);
        obj.insert("cat", msg.category);
        obj.insert("time", QString::number(msg.timestamp));
        obj.insert("msg", msg.message);
        if (!msg.sourcePosition.isEmpty()) {
            obj.insert("src", msg.sourcePosition);
        }
        array.insert(i++, obj);
    }
    return array;
}

QJsonObject Logger::getInformation() {
    QJsonObject info;
    info.insert("fileEnabled", m_fileEnabled);
    info.insert("queueEnabled", m_queueEnabled);
    info.insert("fileCount", getFileCount());
    info.insert("showSourcePos", m_showSource);
    return info;
}
