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

public:
    FileIO() {}
};

#endif // FILEIO_H
