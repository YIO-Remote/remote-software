#ifndef FILEHELPER_H
#define FILEHELPER_H
#include <QObject>
#include <QFile>
#include <QTextStream>

class FileHelper : public QObject {
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = 0) : QObject(parent) {}

public slots:
    bool writeFile(const QString &text) {
        QFile file("/mnt/boot/home-assistant.json");
        if (!file.open(QFile::WriteOnly | QFile::Text)) return false;
        QTextStream out(&file);
        out << text;
        file.close();
        return true;
    }
};

#endif // FILEHELPER_H
