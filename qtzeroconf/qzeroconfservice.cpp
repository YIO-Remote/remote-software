#include <QDebug>
#include "qzeroconfservice.h"

QDebug operator<<(QDebug debug, const QZeroConfService &service)
{
	QDebugStateSaver saver(debug);
	debug.nospace() << "Zeroconf Service: " + service->name() + " @ " + service->host() + " ("+ service->ip().toString()  + ":" + QString::number( service->port()) + ")";
	return debug.maybeSpace();
}

