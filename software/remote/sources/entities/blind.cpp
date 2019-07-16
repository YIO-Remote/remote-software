#include <QtDebug>
#include "blind.h"

QString Blind::Type = "blind";

bool Blind::update(const QVariantMap &attributes)
{
    bool chg = false;
    if (m_state != attributes.value("state").toBool()) {
        m_state = attributes.value("state").toBool();
        chg = true;
        emit stateChanged();
    }
    if (m_position != attributes.value("current_position").toInt()) {
        m_position = attributes.value("current_position").toInt();
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

Blind::Blind() :
    Entity (Type, QVariantMap(), NULL)
{
}

Blind::Blind(const QVariantMap& config, QObject* integrationObj):
    Entity (Type, config, integrationObj)
{
    init(config["attributes"].toMap());
}

void Blind::init(const QVariantMap &config)
{
    m_state = config.value("state").toBool();
    emit stateChanged();

    m_position = config.value("current_position").toInt();
    emit positionChanged();
}
