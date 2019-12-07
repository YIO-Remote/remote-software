#include <iostream>
#include <QDir>
#include "logger.h"
#include "config.h"

Logger*     Logger::s_instance          = nullptr;
QStringList Logger::s_msgTypeString     = { "DEBUG", "WARN ", "CRIT ", "FATAL", "INFO " };      // parallel to QMsgType
QtMsgType   Logger::s_msgTypeSorted[]   = { QtDebugMsg, QtInfoMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg }; // sorted by severity

Logger::Logger(const QString& path, QtMsgType logLevel, bool debug, bool showSource, int queueSize, int purgeHours, QObject *parent) :
    QObject(parent),
    m_debugEnabled(debug),
    m_fileEnabled(path.length() > 0),
    m_queueEnabled(false),
    m_showSource(showSource),
    m_maxQueueSize(queueSize),
    m_directory(path + "/log"),
    m_file(nullptr)
{
    s_instance = this;

    Q_ASSERT(s_msgTypeString.length() == QtMsgType::QtInfoMsg + 1);

    if (!QDir(m_directory).exists())
        QDir().mkdir(m_directory);

    qInstallMessageHandler(&messageOutput);
    Config* config = Config::getInstance ();
    if (config != nullptr) {
        QVariantMap settings = config->getSettings();
        QString log = settings["log"].toString();
        if (!log.isEmpty())
            logLevel = static_cast<QtMsgType>(toMsgType(log));
    }
    defineLogCategory("default", QtMsgType::QtInfoMsg, QLoggingCategory::defaultCategory());
    setLogLevel(logLevel);
    purgeFiles(purgeHours);
}
Logger::~Logger()
{
    if (m_file != nullptr) {
        m_file->close();
    }
    s_instance = nullptr;
}

int Logger::toMsgType (const QString& msgType)
{
    QString mt = msgType.toUpper();
    for (int i = 0; i < s_msgTypeString.length(); i++) {
        if (mt.startsWith(s_msgTypeString[i].trimmed()))
            return i;
    }
    return QtMsgType::QtDebugMsg;
}

void Logger::setLogLevel (int logLevel)
{
    QtMsgType level = static_cast<QtMsgType>(logLevel);
    m_logLevel = level;
    m_logLevelMask = logLevelToMask(level);
}

void Logger::setCategoryLogLevel (const QString& category, int logLevel)
{
    // set integrations loglevel
    defineLogCategory(category, logLevel);
}
void Logger::defineLogCategory (const QString& category, int level, QLoggingCategory* loggingCategory, PluginInterface* plugin)
{
    QtMsgType logLevel = static_cast<QtMsgType>(level);
    SCategory* cat = m_categories.value(category);
    if (cat == nullptr) {
        // Add new category
        cat = new SCategory(logLevel, loggingCategory, plugin);
        m_categories.insert(category, cat);
        qInfo() << "Create logging category" << category << "Level : " << s_msgTypeString[level];
    }
    else {
        if (cat->logLevel != logLevel) {
            qInfo() << "Set logging category" << category << "Level : " << s_msgTypeString[level];
            cat->logLevel = logLevel;
            cat->logLevelMask = logLevelToMask(logLevel);
        }
    }
    bool enable = false;
    int size = sizeof (s_msgTypeSorted) / sizeof (s_msgTypeSorted[0]);
    for (int i = 0; i < size; i++) {
        QtMsgType thisLevel = s_msgTypeSorted[i];
        if (logLevel == thisLevel)
            enable = true;
        if (cat->logCategory != nullptr)
            cat->logCategory->setEnabled(thisLevel, enable);
        if (cat->plugin != nullptr)
            cat->plugin->setLogEnabled(thisLevel, enable);
    }
}
void Logger::removeCategory (const QString& category)
{
    if (m_categories.contains(category))
        m_categories.remove(category);
}

int Logger::getCategoryLogLevel (const QString& category)
{
    SCategory* cat = m_categories.value(category);
    return (cat == nullptr) ? m_logLevel : cat->logLevel;
}

quint16 Logger::logLevelToMask (QtMsgType logLevel)
{
    int size = sizeof (s_msgTypeSorted) / sizeof (s_msgTypeSorted[0]);
    bool enable = false;
    quint16 mask = 0;
    for (int i = 0; i < size; i++) {
        if (logLevel == s_msgTypeSorted[i])
            enable = true;
        if (enable)
            mask |= (1 << i);
    }
    return mask;
}

void Logger::processMessage  (QtMsgType type,  const char* category, const char* source, int line, const QString& msg, bool writeanyHow)
{
    QString cat = category == nullptr ? "default" : category;
    SCategory* c = m_categories.value(cat);
    if (c == nullptr) {
        // Add new category
        c = new SCategory (m_logLevel);     // Initialize with overall log level
        m_categories.insert(cat, c);
    }
    Q_ASSERT(type <= QtMsgType::QtInfoMsg);
    c->count[type]++;
    // if overall or category specific is enabled
    if (writeanyHow || !!((m_logLevelMask | c->logLevelMask) & (1 << type))) {
        QString sourcePosition;
        if (m_showSource && source != nullptr)
            sourcePosition = QString("%1:%2").arg(source).arg(line);
        QDateTime dt = QDateTime::currentDateTime();
        SMessage message(type, dt.toTime_t(), cat, msg, sourcePosition);
        if (m_debugEnabled)
            writeDebug(message);
        if (m_fileEnabled)
            writeFile(message, dt);
        if (m_queueEnabled)
            writeQueue(message);
    }
}

void Logger::messageOutput (::QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
    if (s_instance != nullptr) {
        s_instance->processMessage(type, context.category, context.file, context.line, msg);
    }
}

void Logger::writeFile   (SMessage& message, const QDateTime& dt)
{
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
    QString msg = QString("%1 %2 %3 %4 %5\n\r").arg(dt.toString("dd.MM.yyyy hh:mm:ss")).arg(s_msgTypeString[message.type]).arg(message.category).arg(message.message).arg(message.sourcePosition);
    QTextStream out (m_file);
    out.setCodec("UTF-8");
    out << msg;
}

void Logger::writeQueue  (SMessage& message)
{
    QMutexLocker    lock(&m_queueMutex);
    if (m_queue.count() >= m_maxQueueSize)
        m_queue.dequeue();
    m_queue.enqueue(message);
}

void Logger::writeDebug  (SMessage& message)
{
    QString msg = QString("%1 %2 %3 %4\n").arg(s_msgTypeString[message.type]).arg(message.category).arg(message.message).arg(message.sourcePosition);
    std::cout << msg.toStdString();     // goes to QtCreator console
    std::cout.flush();
}

void Logger::write(const QString& msg)
{
    processMessage(QtMsgType::QtInfoMsg, "default", nullptr, 0, msg, true);
}
void Logger::writeDebug(const QString& msg)
{
    processMessage(QtMsgType::QtDebugMsg, "default", nullptr, 0, msg, true);
}
void Logger::writeInfo(const QString& msg)
{
    processMessage(QtMsgType::QtInfoMsg, "default", nullptr, 0, msg, true);
}
void Logger::writeWarning(const QString& msg)
{
    processMessage(QtMsgType::QtWarningMsg, "default", nullptr, 0, msg, true);
}
void Logger::purgeFiles (int purgeHours)
{
    QDir        dir(m_directory);
    QDateTime   dt = QDateTime::currentDateTime();
    dt = dt.addSecs( - purgeHours * 3600);
    QStringList fileNames = dir.entryList(QStringList("*.log"), QDir::Files);
    int count = 0;
    for (QStringList::iterator i = fileNames.begin(); i != fileNames.end(); ++i) {
        try {
            int idx = i->lastIndexOf('.');
            if (idx > 0) {
                QDateTime filedt = QDateTime::fromString(i->left(idx), "yyyy-MM-dd-HH");
                if (filedt < dt) {
                    dir.remove(*i);
                    count++;
                }
            }
        }
        catch (...) {
            qWarning() << "Error during purge";
        }
    }
    if (count > 0)
        qInfo() << "Files purged : " << count;
}
int Logger::getFileCount ()
{
    QDir        dir(m_directory);
    QStringList fileNames = dir.entryList(QStringList("*.log"), QDir::Files);
    return fileNames.length();
}

QJsonArray Logger::getQueuedMessages (int maxCount, int logLevel, const QStringList& categories)
{
    QJsonArray array;
    int i = 0;
    quint16 levelMask = logLevelToMask(static_cast<QtMsgType>(logLevel));
    while (i < maxCount && !m_queue.isEmpty()) {
        SMessage msg = m_queue.dequeue();
        if (!(levelMask & (1 << msg.type)))
            continue;
        if (categories.length() > 0 && !categories.contains(msg.category))
            continue;
        QJsonObject obj;
        obj.insert("type", msg.type);
        obj.insert("cat", msg.category);
        obj.insert("time", QString::number(msg.timestamp));
        obj.insert("msg", msg.message);
        if (!msg.sourcePosition.isEmpty())
            obj.insert("src", msg.sourcePosition);
        array.insert(i++, obj);
    }
    return array;
}
QJsonObject Logger::getInformation ()
{
    QJsonObject info;
    info.insert("fileEnabled",   m_fileEnabled);
    info.insert("queueEnabled",  m_queueEnabled);
    info.insert("debugEnabled",  m_debugEnabled);
    info.insert("fileCount",     getFileCount());
    info.insert("showSourcePos", m_showSource);
    QJsonArray array;
    QHashIterator<QString, SCategory*> i(m_categories);
    int idx = 0;
    while (i.hasNext()) {
        i.next();
        QJsonObject obj;
        obj.insert("category", i.key());
        obj.insert("level", i.value()->logLevel);
        obj.insert("countDebug", i.value()->count[QtMsgType::QtDebugMsg]);
        obj.insert("countInfo", i.value()->count[QtMsgType::QtInfoMsg]);
        obj.insert("countCritical", i.value()->count[QtMsgType::QtCriticalMsg]);
        obj.insert("countWarning", i.value()->count[QtMsgType::QtWarningMsg]);
        obj.insert("countFatal", i.value()->count[QtMsgType::QtFatalMsg]);
        array.insert(idx++, obj);
    }
    info.insert("categories", array);
    return info;
}


