/******************************************************************************
 *
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

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QLoggingCategory>

class Logger : public QObject
{
    Q_OBJECT


public:
    Q_ENUM(QtMsgType)

    Q_PROPERTY  (QtMsgType logLevel READ logLevel WRITE setLogLevel)

    Q_INVOKABLE void    write          (const QString& msg);
    Q_INVOKABLE void    writeDebug     (const QString& msg);
    Q_INVOKABLE void    writeInfo      (const QString& msg);
    Q_INVOKABLE void    writeWarning   (const QString& msg);

    // path :       directory for log file, if empty no log file
    // logLevel :   default log level, will be overwritten by config setting.log
    // debug :      output to QtCreator DEBUG
    // showSource : show qDebug ... source and line
    explicit Logger (const QString& path, QtMsgType logLevel = QtDebugMsg, bool debug = true, bool showSource = false, QObject *parent = nullptr);
    ~Logger();

    QtMsgType           logLevel        ()
    { return m_logLevel; }

    void                setLogLevel     (QtMsgType msgType);

    static Logger*      getInstance     ()
    { return s_instance; }

private:
    void                output          (const QString& msg);
    void                writeFile       (const QString& msg);
    void                messageOutput   (QtMsgType type,  const QMessageLogContext &context, const QString& msg);

    static void         s_messageOutput (QtMsgType type, const QMessageLogContext &context, const QString& msg)
    {  
        if (s_instance != nullptr)
            s_instance->messageOutput (type, context, msg); 
    }

    static Logger*      s_instance;
    static QStringList  s_msgTypeString;

    QtMsgType           m_logLevel;     // log level
    bool                m_debug;        // output to debug m,ode
    bool                m_showSource;   // Show source file and line
    int                 m_lastHour;     // Every hour we create a new file
    QLoggingCategory    m_logCat;       // Used for QML logs
    QString             m_directory;    // Fore files
    QFile*              m_file;
};

#endif // LOGGER_H
