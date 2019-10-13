#include <QtDebug>
#include "blind.h"

QString Blind::Type = "blind";

bool Blind::update(const QVariantMap &attributes)
{
    bool chg = false;
    if (attributes.contains("state") && m_state != attributes.value("state").toBool()) {
        m_state = attributes.value("state").toBool();
        chg = true;
        emit stateChanged();
    }
    if (attributes.contains("position") && m_position != attributes.value("position").toInt()) {
        m_position = attributes.value("position").toInt();
        chg = true;
        emit positionChanged();
    }
    return chg;
}

void Blind::close()
{
    command("CLOSE", "");
}

void Blind::open()
{
    command("OPEN", "");
}

void Blind::stop()
{
    command("STOP", "");
}

void Blind::setPosition(int value)
{
     command("POSITION", value);
}

Blind::Blind(QObject *parent) :
    Entity (Type, QVariantMap(), NULL, parent)
{
}

Blind::Blind(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
}
