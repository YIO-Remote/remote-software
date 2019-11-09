#include "utils.h"

#include <QDebug>

Utils* Utils::s_instance = NULL;

Utils::Utils(QObject *parent) : QObject(parent)
{
    s_instance = this;

}

Utils::~Utils()
{
    s_instance = NULL;
}
