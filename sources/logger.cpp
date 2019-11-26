#include "logger.h"
#include <iostream>
#include "config.h"

Logger*     Logger::s_instance      = nullptr;
QStringList Logger::s_msgTypeString = { "DEBUG", "WARN ", "CRIT ", "FATAL", "INFO " };      // parallel to QMsgType

Logger::Logger(const QString& path, QtMsgType logLevel, bool debug, bool showSource, QObject *parent) :
    QObject(parent),
    m_debug(debug),
    m_showSource(showSource),
    m_logCat("yio"),
    m_directory(path),
    m_file(nullptr)
{
    s_instance = this;

    qInstallMessageHandler(s_messageOutput);
    Config* config = Config::getInstance ();
    if (config != nullptr) {
        const QVariantMap& map = config->config();
        QVariantMap settings = map["settings"].toMap();
        QString log = settings["log"].toString().toLower();
        if (!log.isEmpty()) {
            if (log == "debug")
                logLevel = QtDebugMsg;
            else if (log == "info")
                logLevel = QtInfoMsg;
            else if (log == "warning")
                logLevel = QtWarningMsg;
        }
     }

    setLogLevel(logLevel);
}
Logger::~Logger() {
    if (m_file != nullptr) {
        m_file->close();
    }

    s_instance = nullptr;
}

void Logger::setLogLevel (QtMsgType msgType)
{
    // enable msg types "">" msgType
    QtMsgType msgTypes[] = { QtDebugMsg, QtInfoMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg };
    int size = sizeof (msgTypes) / sizeof (msgTypes[0]);

    bool enable = false;
    for (int i = 0; i < size; i++) {
        if (msgType == msgTypes[i])
            enable = true;
        m_logCat.setEnabled(msgTypes[i], enable);
    }
}

void Logger:: messageOutput (QtMsgType type,  const QMessageLogContext &context, const QString &msg)
{
    QString category;
    if (context.category && strcmp(context.category, "default")) {
        // has its on category where msgtypes are enabled / disabled
        category = context.category;
    }
    else {
        // no category, use "yio" category
        if (!m_logCat.isEnabled(type))
            return;
        category = m_logCat.categoryName();
    }
    QString file = context.file ? context.file : "";
    QString formattedMsg;
    if (m_showSource) {
        QString file = context.file ? context.file : "";
        formattedMsg = QString("%1: %2 %3 (%4/%5)").arg (s_msgTypeString[type], category, msg, file, QString::number(context.line));
    }
    else
        formattedMsg = QString("%1: %2 %3").arg (s_msgTypeString[type], category, msg);
    output (formattedMsg);
}

void Logger::output(const QString& msg)
{
    if (!m_directory.isEmpty())
        writeFile (msg);
    if (m_debug) {
        std::cout << msg.toStdString() << "\n";     // goes to QtCreator console
        std::cout.flush();
    }
}

void Logger::write(const QString& msg)
{
    output ("     : qml " + msg);
}
void Logger::writeDebug(const QString& msg)
{
    if (m_logCat.isEnabled(QtDebugMsg))
        output (s_msgTypeString[QtDebugMsg] + ": qml " + msg);
}
void Logger::writeInfo(const QString& msg)
{
    if (m_logCat.isEnabled(QtInfoMsg))
        output (s_msgTypeString[QtInfoMsg] + ": qml " + msg);
}
void Logger::writeWarning(const QString& msg)
{
    if (m_logCat.isEnabled(QtWarningMsg))
        output (s_msgTypeString[QtWarningMsg] + ": qml " + msg);
}
void Logger::writeFile(const QString& msg)
{
    QDateTime dt = QDateTime::currentDateTime();
    int hour = dt.time().hour();
    if (hour != m_lastHour || m_file == nullptr) {
        m_lastHour = hour;
        if (m_file != nullptr)
            m_file->close();
        m_file = new QFile;
        QString path = QString("%1/%2.log").arg(m_directory, dt.toString("yyyy-MM-dd-hh"));
        m_file->setFileName(path);
        m_file->open(QIODevice::Append | QIODevice::Text);
    }
    QString text = dt.toString("dd.MM.yyyy hh:mm:ss ") + msg + "\n\r";
    QTextStream out (m_file);
    out.setCodec("UTF-8");
    out << text;
}
