#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class Logger : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE void    write          (const QString& msg);
    Q_INVOKABLE void    writeDebug     (const QString& msg);
    Q_INVOKABLE void    writeInfo      (const QString& msg);
    Q_INVOKABLE void    writeWarning   (const QString& msg);

    explicit Logger (QObject *parent, QString path, QtMsgType logLevel = QtDebugMsg, bool debug = true, bool showFile = false);
    ~Logger();

    void                setLogLevel     (QtMsgType msgType);

    static Logger*      getInstance     ()
    { return s_instance; }

private:
    void                output          (const QString& msg);
    void                writeFile       (const QString& msg);
    void                messageOutput   (QtMsgType type,  const QMessageLogContext &context, const QString& msg);

    static void         s_messageOutput (QtMsgType type, const QMessageLogContext &context, const QString& msg)
    {  s_instance->messageOutput (type, context, msg); }

    static Logger*      s_instance;
    static QStringList  s_msgTypeString;

    bool                m_debug;
    bool                m_showFile;
    QFile*              m_file;
    quint16             m_logLevelMask;
};

#endif // LOGGER_H
