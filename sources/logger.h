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
    Q_INVOKABLE void    purgeFiles     (int purgeHours);

    // path :       directory for log file, if empty no log file
    // logLevel :   default log level, will be overwritten by config setting.log
    // debug :      output to QtCreator DEBUG
    // showSource : show qDebug ... source and line
    explicit Logger (const QString& path, QtMsgType logLevel = QtDebugMsg, bool debug = true, bool showSource = false, int purgeHours = 12, QObject *parent = nullptr);
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
