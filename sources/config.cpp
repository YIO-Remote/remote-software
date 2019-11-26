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

QObject *Config::getQMLObject(QList<QObject*> nodes,const QString &name)
{
    for (int i=0; i < nodes.size(); i++)
    {
        if (nodes.at(i) && nodes.at(i)->objectName() == name) {
            return dynamic_cast<QObject*>(nodes.at(i));
        } else if (nodes.at(i) && nodes.at(i)->children().size() > 0) {
            QObject* item = getQMLObject(nodes.at(i)->children(), name);
            if (item)
                return item;
        }
    }
    return  nullptr;
}

QObject *Config::getQMLObject(const QString &name)
{
    return getQMLObject(m_engine->rootObjects(), name);
}
