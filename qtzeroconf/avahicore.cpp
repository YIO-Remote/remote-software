/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (C) 2015  Jonathan Bagg
	This file is part of QtZeroConf.

	QtZeroConf is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	QtZeroConf is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with QtZeroConf.  If not, see <http://www.gnu.org/licenses/>.
---------------------------------------------------------------------------------------------------
   Project name : QtZeroConf
   File name    : avahicore.cpp
   Created      : 9 September 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Avahi-core wrapper for use in embedded Linux systems (Android)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
//#include <avahi-qt4/qt-watch.h>	//
#include "avahi-qt/qt-watch.h"		// fixme don't depend on avahi-qt until it supports Qt5
#include <avahi-core/core.h>
#include <avahi-core/publish.h>
#include <avahi-core/lookup.h>
#include <avahi-common/simple-watch.h>
#include <QCoreApplication>
#include "qzeroconf.h"

class QZeroConfPrivate
{
public:
	QZeroConfPrivate(QZeroConf *parent)
	{
		qint32 error;

		pub = parent;
		group = NULL;
		browser = NULL;
		txt = NULL;
		ready = 0;
		registerWaiting = 0;

		poll = avahi_qt_poll_get();
		if (!poll) {
			return;
		}

		avahi_server_config_init(&config);
		config.publish_workstation = 0;

		if (!referenceCount) {
			server = avahi_server_new(poll, &config, serverCallback, this, &error);
		}
		referenceCount++;
		if (!server) {
			return;
		}
	}

	static void serverCallback(AvahiServer *, AvahiServerState state, AVAHI_GCC_UNUSED void * userdata)
	{
		QZeroConfPrivate *ref = static_cast<QZeroConfPrivate *>(userdata);
		switch (state) {
			case AVAHI_SERVER_RUNNING:
				ref->ready = 1;
                if (ref->registerWaiting) {
                    ref->registerWaiting = 0;
                    ref->registerService(ref->name.toUtf8(), ref->type.toUtf8(), ref->domain.toUtf8(), ref->port);
				}
				break;
			case AVAHI_SERVER_COLLISION:
				break;
			case AVAHI_SERVER_REGISTERING: break;
			case AVAHI_SERVER_FAILURE:
				break;
			case AVAHI_SERVER_INVALID: break;
		}
	}

	static void groupCallback(AvahiServer *, AvahiSEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata)
	{
		QZeroConfPrivate *ref = static_cast<QZeroConfPrivate *>(userdata);
		switch (state) {
			case AVAHI_ENTRY_GROUP_ESTABLISHED:
				emit ref->pub->servicePublished();
				break;
			case AVAHI_ENTRY_GROUP_COLLISION:
				avahi_s_entry_group_free(g);
				ref->group = NULL;
				emit ref->pub->error(QZeroConf::serviceNameCollision);
				break;
			case AVAHI_ENTRY_GROUP_FAILURE :
				avahi_s_entry_group_free(g);
				ref->group = NULL;
				emit ref->pub->error(QZeroConf::serviceRegistrationFailed);
				break;
			case AVAHI_ENTRY_GROUP_UNCOMMITED: break;
			case AVAHI_ENTRY_GROUP_REGISTERING: break;
		}
	}

	static void browseCallback(
			AvahiSServiceBrowser *,
			AvahiIfIndex interface,
			AvahiProtocol protocol,
			AvahiBrowserEvent event,
			const char *name,
			const char *type,
			const char *domain,
			AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
			void* userdata)
	{
		QString key = name + QString::number(interface);
		QZeroConfPrivate *ref = static_cast<QZeroConfPrivate *>(userdata);
		QZeroConfService zcs;

		switch (event) {
			case AVAHI_BROWSER_FAILURE:
				ref->broswerCleanUp();
				emit ref->pub->error(QZeroConf::browserFailed);
				break;
			case AVAHI_BROWSER_NEW:
				if (!ref->resolvers.contains(key))
					ref->resolvers.insert(key, avahi_s_service_resolver_new(ref->server, interface, protocol, name, type, domain, ref->aProtocol, AVAHI_LOOKUP_USE_MULTICAST, resolveCallback, ref));
				break;
			case AVAHI_BROWSER_REMOVE:
				if (!ref->resolvers.contains(key))
					return;
				avahi_s_service_resolver_free(ref->resolvers[key]);
				ref->resolvers.remove(key);

				if (!ref->pub->services.contains(key))
					return;
				zcs = ref->pub->services[key];
				ref->pub->services.remove(key);
				emit ref->pub->serviceRemoved(zcs);
				break;
			case AVAHI_BROWSER_ALL_FOR_NOW:
			case AVAHI_BROWSER_CACHE_EXHAUSTED:
				break;
		}
	}

	static void resolveCallback(
	    AVAHI_GCC_UNUSED AvahiSServiceResolver *r,
	    AVAHI_GCC_UNUSED AvahiIfIndex interface,
	    AVAHI_GCC_UNUSED AvahiProtocol protocol,
	    AvahiResolverEvent event,
	    const char *name,
	    const char *type,
	    const char *domain,
	    const char *host_name,
	    const AvahiAddress *address,
	    uint16_t port,
	    AvahiStringList *txt,
	    AvahiLookupResultFlags,
	    AVAHI_GCC_UNUSED void* userdata)
	{
		bool newRecord = 0;
		QZeroConfService zcs;
		QZeroConfPrivate *ref = static_cast<QZeroConfPrivate *>(userdata);

		QString key = name + QString::number(interface);
		if (event == AVAHI_RESOLVER_FOUND) {
			if (ref->pub->services.contains(key))
				zcs = ref->pub->services[key];
			else {
				zcs = QZeroConfService(new QZeroConfServiceData);
				newRecord = 1;
				zcs->m_name = name;
				zcs->m_type = type;
				zcs->m_domain = domain;
				zcs->m_host = host_name;
				zcs->m_interfaceIndex = interface;
				zcs->m_port = port;
				while (txt)	// get txt records
				{
					QByteArray avahiText((const char *)txt->text, txt->size);
					QList<QByteArray> pair = avahiText.split('=');
					if (pair.size() == 2)
						zcs->m_txt[pair.at(0)] = pair.at(1);
					else
						zcs->m_txt[pair.at(0)] = "";
					txt = txt->next;
				}
				ref->pub->services.insert(key, zcs);
			}

			char a[AVAHI_ADDRESS_STR_MAX];
			avahi_address_snprint(a, sizeof(a), address);
			QHostAddress addr(a);
			zcs->setIp(addr);

			if (newRecord)
				emit ref->pub->serviceAdded(zcs);
			else
				emit ref->pub->serviceUpdated(zcs);
		}
		else if (ref->pub->services.contains(key)) {	// delete service if exists and unable to resolve
			zcs = ref->pub->services[key];
			ref->pub->services.remove(key);
			emit ref->pub->serviceRemoved(zcs);
			// don't delete the resolver here...we need to keep it around so Avahi will keep updating....might be able to resolve the service in the future
		}
	}

	void broswerCleanUp(void)
	{
		if (!browser)
			return;
		avahi_s_service_browser_free(browser);
		browser = NULL;

		QMap<QString, QZeroConfService>::iterator i;
		for (i = pub->services.begin(); i != pub->services.end(); i++) {
			emit pub->serviceRemoved(i.value());
		}
		pub->services.clear();

		QMap<QString, AvahiSServiceResolver *>::iterator r;
		for (r = resolvers.begin(); r != resolvers.end(); r++)
			avahi_s_service_resolver_free(*r);
		resolvers.clear();
	}

	void registerService(const char *name, const char *type, const char *domain, quint16 port)
	{
		qint32 ret;
		group = avahi_s_entry_group_new(server, QZeroConfPrivate::groupCallback, this);
		if (!group) {
			pub->emit error(QZeroConf::serviceRegistrationFailed);
			return;
		}

		ret = avahi_server_add_service_strlst(server, group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, AVAHI_PUBLISH_UPDATE, name, type, domain, NULL, port, txt);
		if (ret < 0) {
			avahi_s_entry_group_free(group);
			group = NULL;
			pub->emit error(QZeroConf::serviceRegistrationFailed);
			return;
		}

		ret = avahi_s_entry_group_commit(group);
		if (ret < 0) {
			avahi_s_entry_group_free(group);
			group = NULL;
			pub->emit error(QZeroConf::serviceRegistrationFailed);
		}
	}

	QZeroConf *pub;
	const AvahiPoll *poll;
	static AvahiServer *server;
	static quint32 referenceCount;
	AvahiServerConfig config;
	AvahiSEntryGroup *group;
	AvahiSServiceBrowser *browser;
	AvahiProtocol aProtocol;
	QMap <QString, AvahiSServiceResolver *> resolvers;
	AvahiStringList *txt;
	bool ready, registerWaiting;
	QString name, type, domain;
	qint32 port;
};

AvahiServer* QZeroConfPrivate::server = nullptr;
quint32 QZeroConfPrivate::referenceCount = 0;

QZeroConf::QZeroConf(QObject *parent) : QObject (parent)
{
	pri = new QZeroConfPrivate(this);
	qRegisterMetaType<QZeroConfService>("QZeroConfService");
}

QZeroConf::~QZeroConf()
{
	avahi_string_list_free(pri->txt);
	pri->broswerCleanUp();
	avahi_server_config_free(&pri->config);
	pri->referenceCount--;
	if (!pri->referenceCount)
		avahi_server_free(pri->server);
	delete pri;
}

void QZeroConf::startServicePublish(const char *name, const char *type, const char *domain, quint16 port)
{
	if (pri->group) {
		emit error(QZeroConf::serviceRegistrationFailed);
		return;
	}
	if (pri->ready)
		pri->registerService(name, type, domain, port);
	else {
		pri->registerWaiting = 1;
		pri->name = name;
		pri->type = type;
		pri->domain = domain;
		pri->port = port;
	}
}

void QZeroConf::stopServicePublish(void)
{
	if (pri->group) {
		avahi_s_entry_group_free(pri->group);
		pri->group = NULL;
	}
}

bool QZeroConf::publishExists(void)
{
	if (pri->group)
		return true;
	else
		return false;
}

// http://www.zeroconf.org/rendezvous/txtrecords.html

void QZeroConf::addServiceTxtRecord(QString nameOnly)
{
	pri->txt = avahi_string_list_add(pri->txt, nameOnly.toUtf8());
}

void QZeroConf::addServiceTxtRecord(QString name, QString value)
{
	name.append("=");
	name.append(value);
	addServiceTxtRecord(name);
}

void QZeroConf::clearServiceTxtRecords()
{
	avahi_string_list_free(pri->txt);
	pri->txt = NULL;
}

void QZeroConf::startBrowser(QString type, QAbstractSocket::NetworkLayerProtocol protocol)
{
	if (pri->browser)
		emit error(QZeroConf::browserFailed);

	switch (protocol) {
		case QAbstractSocket::IPv4Protocol: pri->aProtocol = AVAHI_PROTO_INET; break;
		case QAbstractSocket::IPv6Protocol: pri->aProtocol = AVAHI_PROTO_INET6; break;
		default:
			qDebug("QZeroConf::startBrowser() - unsupported protocol, using IPv4");
			pri->aProtocol = AVAHI_PROTO_INET;
			break;
	};

	pri->browser = avahi_s_service_browser_new(pri->server, AVAHI_IF_UNSPEC, pri->aProtocol, type.toUtf8(), NULL, AVAHI_LOOKUP_USE_MULTICAST, QZeroConfPrivate::browseCallback, pri);
	if (!pri->browser)
		emit error(QZeroConf::browserFailed);
}

void QZeroConf::stopBrowser(void)
{
	pri->broswerCleanUp();
}

bool QZeroConf::browserExists(void)
{
	if (pri->browser)
		return true;
	else
		return false;
}
