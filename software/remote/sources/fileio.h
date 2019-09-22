#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class FileIO : public QObject
{
    Q_OBJECT

public slots:
    bool write(const QString& source, const QString& data)
    {
        if (source.isEmpty())
            return false;

        QFile file(source);
        if (!file.open(QFile::WriteOnly | QFile::Truncate))
            return false;

        QTextStream out(&file);
        out << data;
        file.close();
        return true;
    }

    QString read(const QString& source)
    {
        QString returnText;

        QFile file(source);
        if (!file.exists()) {
            return QString("");
        }

        if (file.open(QFile::ReadOnly)) {
            QTextStream stream(&file);
            returnText = stream.readAll();
        }

        return returnText;
    }

    bool exists(const QString& source)
    {
        QFile file(source);

        if (file.exists()) {
            return true;
        } else {
            return false;
        }
    }

public:
    FileIO() {}
};

#endif // FILEIO_H
