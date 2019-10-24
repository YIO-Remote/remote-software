#include "logger.h"

Logger* Logger::s_instance = nullptr;

Logger::Logger(QObject *parent, QString path) : QObject(parent)
{
    s_instance = this;

    m_file = new QFile;
    m_file->setFileName(path + "/log");
    m_file->open(QIODevice::Append | QIODevice::Text);
}

Logger::~Logger() {
    if (m_file != 0) {
        m_file->close();
    }

    s_instance = nullptr;
}

void Logger::write(QString msg)
{
    QString text = msg;
    text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + text;
    QTextStream out (m_file);
    out.setCodec("UTF-8");
    if (m_file != 0) {
        out << text;
    }
}
