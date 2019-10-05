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
   File name    : bonjour_p.h
   Created      : 22 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Part of wrapper for Apple's Bonjour library for use on Windows, MACs and iOS.  Needed for slots.
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef QZEROCONFPRIVATE_H_
#define QZEROCONFPRIVATE_H_

#include <dns_sd.h>
#include <QSocketNotifier>
#include <QtEndian>
#include <QHostAddress>
#include "qzeroconf.h"
#include <QDebug>

#ifndef kDNSServiceFlagsTimeout		// earlier versions of dns_sd.h don't define this constant
	#define	kDNSServiceFlagsTimeout	0x10000
#endif

class Resolver : public QObject
{
	Q_OBJECT
public:
	void cleanUp();
	QZeroConfService zcs;
	QZeroConfPrivate *ref = nullptr;
	DNSServiceRef DNSresolverRef = nullptr;
	DNSServiceRef DNSaddressRef = nullptr;
	QSocketNotifier *resolverNotifier = nullptr;
	QSocketNotifier *addressNotifier = nullptr;

public slots:
	void resolverReady();
	void addressReady();
};

class QZeroConfPrivate : public QObject
{
	Q_OBJECT

public:
	QZeroConfPrivate(QZeroConf *parent);
	void cleanUp(DNSServiceRef ref);
	void resolve(QZeroConfService);

	static void DNSSD_API registerCallback(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *,
			const char *, const char *, void *userdata);

	static void DNSSD_API browseCallback(DNSServiceRef, DNSServiceFlags flags,quint32, DNSServiceErrorType err, const char *name,
			const char *type, const char *domain, void *userdata);

	static void DNSSD_API resolverCallback(DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType err, const char *,
			const char *hostName, quint16 port, quint16 txtLen,	const char * txtRecord, void *userdata);

	static void DNSSD_API addressReply(DNSServiceRef sdRef,	DNSServiceFlags flags, quint32 interfaceIndex,
			DNSServiceErrorType err, const char *hostName, const struct sockaddr* address, quint32 ttl, void *userdata);

	QZeroConf *pub;
	DNSServiceRef dnssRef, browser;
	DNSServiceProtocol protocol;
	QSocketNotifier *bs, *browserSocket;
	QByteArray txt;
	QHash<QString, Resolver*> resolvers;

public slots:
	void bsRead();
	void browserRead();
};

#endif	// QZEROCONFPRIVATE_H_
