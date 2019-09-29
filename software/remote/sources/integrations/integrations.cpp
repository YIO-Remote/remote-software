#include "integrations.h"

Integrations* Integrations::s_instance = NULL;

Integrations::Integrations(QObject *parent) : QObject(parent)
{
    s_instance = this;
}

Integrations::~Integrations()
{
    s_instance = NULL;
}

QList<QObject *> Integrations::list()
{
    return m_integrations.values();
}

void Integrations::add(QString name, QObject *obj, QString friendlyName)
{
    m_integrations.insert(name, obj);
    m_integrations_friendly_names.insert(name, friendlyName);
    emit listChanged();
}

void Integrations::remove(QString name)
{
    m_integrations.remove(name);
    m_integrations_friendly_names.remove(name);
    emit listChanged();
}

QString Integrations::getFriendlyName(QString name)
{
    return m_integrations_friendly_names.value(name);
}
