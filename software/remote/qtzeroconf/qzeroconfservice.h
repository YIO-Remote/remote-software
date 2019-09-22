#ifndef QZEROCONFSERVICE_H
#define QZEROCONFSERVICE_H

#include <QHostAddress>
#include <QMutexLocker>
#include <QSharedPointer>
#include "qzeroconfglobal.h"

class QZeroConfPrivate;

class Q_ZEROCONF_EXPORT QZeroConfServiceData
{
	Q_GADGET
	Q_PROPERTY( QString name READ name )
	Q_PROPERTY( QString type READ type )
	Q_PROPERTY( QString domain READ domain )
	Q_PROPERTY( QString host READ host )

friend class QZeroConfPrivate;

public:
	inline QString name() const {return m_name;}
	inline QString type() const {return m_type;}
	inline QString domain() const {return m_domain;}
	inline QString host() const {return m_host;}
	QHostAddress ip()
	{
		QMutexLocker locker(&m_lock);
		return m_ip;
	}
	inline quint32 interfaceIndex() const {return m_interfaceIndex;}
	quint16 port() const {return m_port;}
	QMap <QByteArray, QByteArray> txt() const {return m_txt;}

private:
	void setIp(QHostAddress &ip)
	{
		QMutexLocker locker(&m_lock);
		m_ip = ip;
	}
	QString			m_name;
	QString			m_type;
	QString			m_domain;
	QString			m_host;
	QHostAddress	m_ip;
	quint32			m_interfaceIndex;
	quint16			m_port;
	QMap			<QByteArray, QByteArray> m_txt;
	QMutex			m_lock;
};

typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

Q_DECLARE_METATYPE(QZeroConfService)

QDebug operator<<(QDebug debug, const QZeroConfService &service);

#endif // QZEROCONFSERVICE_H
