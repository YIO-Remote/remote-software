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
    Q_INVOKABLE void    write   (QString msg);

    explicit Logger(QObject *parent, QString path);
    ~Logger();

    static Logger*       getInstance     ()
    { return s_instance; }

private:
    QFile*  m_file;

    static Logger*               s_instance;
};

#endif // LOGGER_H
