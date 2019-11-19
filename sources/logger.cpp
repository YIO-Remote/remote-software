#include "logger.h"
#include <iostream>

Logger*     Logger::s_instance      = nullptr;
QStringList Logger::s_msgTypeString = { "DEBUG", "WARN ", "CRIT ", "FATAL", "INFO " };      // parallel to QMsgType

Logger::Logger(QObject *parent, QString path, QtMsgType logLevel, bool debug, bool showFile) : QObject(parent)
{
    s_instance = this;

    qInstallMessageHandler(s_messageOutput);
    m_debug = debug;
    m_showFile = showFile;
    setLogLevel(logLevel);

    if (path.isEmpty())
        m_file = nullptr;
    else {
        m_file = new QFile;
        m_file->setFileName(path + "/log");
        m_file->open(QIODevice::Append | QIODevice::Text);
    }
}
Logger::~Logger() {
    if (m_file != nullptr) {
        m_file->close();
    }

    s_instance = nullptr;
}

void Logger::setLogLevel     (QtMsgType msgType)
{
    // unfortunately the QtMsgType is not sorted as reasonable, we have to do it this way
    switch (msgType) {
        case QtDebugMsg:    m_logLevelMask = (1 << QtDebugMsg) | (1 << QtInfoMsg) | (1 << QtWarningMsg) | (1 << QtCriticalMsg) | (1 << QtFatalMsg); break;
        case QtInfoMsg:     m_logLevelMask = (1 << QtInfoMsg) | (1 << QtWarningMsg) | (1 << QtCriticalMsg) | (1 << QtFatalMsg); break;
        case QtWarningMsg:  m_logLevelMask = (1 << QtWarningMsg) | (1 << QtCriticalMsg) | (1 << QtFatalMsg); break;
        case QtCriticalMsg: m_logLevelMask = (1 << QtCriticalMsg) | (1 << QtFatalMsg); break;
        case QtFatalMsg:    m_logLevelMask = (1 << QtFatalMsg); break;
    }
}

void Logger:: messageOutput   (QtMsgType type,  const QMessageLogContext &context, const QString &msg)
{
    if (!(m_logLevelMask & (1 << type)))
        return;

    QString file = context.file ? context.file : "";
    QString category = context.category ? context.category : "";
    QString formattedMsg;
    if (m_showFile)
        formattedMsg = QString("%1: %2 %3 (%4/%5)").arg (s_msgTypeString[type], category, msg, file, QString::number(context.line));
    else
        formattedMsg = QString("%1: %2 %3").arg (s_msgTypeString[type], category, msg);
    output (formattedMsg);
}

void Logger::output(const QString& msg)
{
    if (m_file != nullptr)
        writeFile (msg);
    if (m_debug) {
        std::cout << msg.toStdString() << "\n";
        std::cout.flush();
    }
}

void Logger::write(const QString& msg)
{
    output ("     : QML " + msg);
}
void Logger::writeDebug(const QString& msg)
{
    if (m_logLevelMask & (1 << QtDebugMsg))
        output (s_msgTypeString[QtDebugMsg] + ": QML " + msg);
}
void Logger::writeInfo(const QString& msg)
{
    if (m_logLevelMask & (1 << QtInfoMsg))
        output (s_msgTypeString[QtInfoMsg] + ": QML " + msg);
}
void Logger::writeWarning(const QString& msg)
{
    if (m_logLevelMask & (1 << QtWarningMsg))
        output (s_msgTypeString[QtWarningMsg] + ": QML " + msg);
}
void Logger::writeFile(const QString& msg)
{
    QString text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + msg + "\r\n";
    QTextStream out (m_file);
    out.setCodec("UTF-8");
    if (m_file != nullptr)
        out << text;
}
