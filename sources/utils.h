#ifndef UTILS_H
#define UTILS_H

#include <QObject>

class Utils : public QObject
{
    Q_OBJECT

public:
    explicit Utils(QObject *parent = nullptr);
    virtual ~Utils();

    static Utils*               getInstance ()
    { return s_instance; }

private:
    static Utils*               s_instance;

};
#endif // UTILS_H
