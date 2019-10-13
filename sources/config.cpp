#include "config.h"

Config* Config::s_instance = nullptr;

Config::Config(QQmlApplicationEngine *engine, QString path) :
    m_engine(engine)
{

    s_instance = this;

    //load the config file
    readConfig(path);
}

Config::~Config()
{
    s_instance = nullptr;
}

void Config::readConfig(QString path)
{
    // load the config.json file from the filesystem
    m_jsf->setName(path + "/config.json");
    m_config = m_jsf->read().toMap();
    emit configChanged();
}

void Config::writeConfig()
{
    m_jsf->write(m_config);
}
