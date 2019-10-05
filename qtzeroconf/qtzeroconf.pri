
# for Qt4 on Linux
lessThan(QT_MAJOR_VERSION, 5) {
	HEADERS+= $$PWD/qzeroconf.h $$PWD/avahi-qt/qt-watch.h  $$PWD/avahi-qt/qt-watch_p.h
	SOURCES+= $$PWD/avahiclient.cpp $$PWD/avahi-qt/qt-watch.cpp
	LIBS+= -lavahi-client -lavahi-common
	QMAKE_CXXFLAGS+= -I$$PWD
}

# below for >= Qt5
linux:!android {
	HEADERS+= $$PWD/qzeroconf.h $$PWD/avahi-qt/qt-watch.h  $$PWD/avahi-qt/qt-watch_p.h
	SOURCES+= $$PWD/avahiclient.cpp $$PWD/avahi-qt/qt-watch.cpp
	LIBS+= -lavahi-client -lavahi-common
	QMAKE_CXXFLAGS+= -I$$PWD
}

freebsd {
	HEADERS+= $$PWD/qzeroconf.h $$PWD/avahi-qt/qt-watch.h  $$PWD/avahi-qt/qt-watch_p.h
	SOURCES+= $$PWD/avahiclient.cpp $$PWD/avahi-qt/qt-watch.cpp
	LIBS+= -lavahi-client -lavahi-common
	QMAKE_CXXFLAGS+= -I$$PWD
}

win32 {
	DEFINES+= WIN32
	DEFINES+= NDEBUG
	DEFINES+= _WINDOWS
	DEFINES+= _USRDLL
	DEFINES+= MDNS_DEBUGMSGS=0
	DEFINES+= WIN32_LEAN_AND_MEAN
	DEFINES+= USE_TCP_LOOPBACK
	DEFINES+= _CRT_SECURE_NO_DEPRECATE
	DEFINES+= _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
	DEFINES+= NOT_HAVE_SA_LEN
	QMAKE_CFLAGS+= -I$$PWD/bonjour-sdk
	QMAKE_CXXFLAGS+= -I$$PWD/bonjour-sdk -I$$PWD
	HEADERS+= $$PWD/qzeroconf.h $$PWD/bonjour_p.h
	SOURCES+= $$PWD/bonjour.cpp
	SOURCES+= $$PWD/bonjour-sdk/dnssd_clientlib.c
	SOURCES+= $$PWD/bonjour-sdk/dnssd_clientstub.c
	SOURCES+= $$PWD/bonjour-sdk/dnssd_ipc.c
	LIBS+= -lws2_32 -lwsock32
}

macx {
	HEADERS+= $$PWD/qzeroconf.h $$PWD/bonjour_p.h
	SOURCES+= $$PWD/bonjour.cpp
	QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreServices.framework/
	LIBS+= -framework CoreServices
	QMAKE_CXXFLAGS+= -I$$PWD
}

ios {
	HEADERS+= $$PWD/qzeroconf.h $$PWD/bonjour_p.h
	SOURCES+= $$PWD/bonjour.cpp
	QMAKE_CXXFLAGS+= -I$$PWD
}

android {
	QMAKE_CXXFLAGS+= -I$$PWD
	QMAKE_CFLAGS+= -I$$PWD
	ACM = $$PWD/avahi-common
	ACR = $$PWD/avahi-core
	HEADERS+= $$PWD/qzeroconf.h $$PWD/avahi-qt/qt-watch.h  $$PWD/avahi-qt/qt-watch_p.h
	SOURCES+= $$PWD/avahicore.cpp $$PWD/avahi-qt/qt-watch.cpp
	# avahi-common
	DEFINES+= HAVE_STRLCPY GETTEXT_PACKAGE
	SOURCES+= $$ACM/address.c
	SOURCES+= $$ACM/alternative.c
	SOURCES+= $$ACM/domain.c
	SOURCES+= $$ACM/error.c
	SOURCES+= $$ACM/i18n.c
	SOURCES+= $$ACM/malloc.c
	SOURCES+= $$ACM/rlist.c
	SOURCES+= $$ACM/simple-watch.c
	SOURCES+= $$ACM/strlst.c
	SOURCES+= $$ACM/thread-watch.c
	SOURCES+= $$ACM/timeval.c
	SOURCES+= $$ACM/utf8.c
	# avahi-core
	DEFINES+= HAVE_NETLINK
	SOURCES+= $$ACR/addr-util.c
	SOURCES+= $$ACR/announce.c
	SOURCES+= $$ACR/browse.c
	SOURCES+= $$ACR/browse-dns-server.c
	SOURCES+= $$ACR/browse-domain.c
	SOURCES+= $$ACR/browse-service.c
	SOURCES+= $$ACR/browse-service-type.c
	SOURCES+= $$ACR/cache.c
	SOURCES+= $$ACR/dns.c
	SOURCES+= $$ACR/domain-util.c
	SOURCES+= $$ACR/entry.c
	SOURCES+= $$ACR/fdutil.c
	SOURCES+= $$ACR/hashmap.c
	SOURCES+= $$ACR/iface.c
	SOURCES+= $$ACR/iface-linux.c
	SOURCES+= $$ACR/log.c
	SOURCES+= $$ACR/multicast-lookup.c
	SOURCES+= $$ACR/netlink.c
	SOURCES+= $$ACR/prioq.c
	SOURCES+= $$ACR/probe-sched.c
	SOURCES+= $$ACR/querier.c
	SOURCES+= $$ACR/query-sched.c
	SOURCES+= $$ACR/resolve-address.c
	SOURCES+= $$ACR/resolve-host-name.c
	SOURCES+= $$ACR/resolve-service.c
	SOURCES+= $$ACR/response-sched.c
	SOURCES+= $$ACR/rr.c
	SOURCES+= $$ACR/rrlist.c
	SOURCES+= $$ACR/server.c
	SOURCES+= $$ACR/socket.c
	SOURCES+= $$ACR/timeeventq.c
	SOURCES+= $$ACR/util.c
	SOURCES+= $$ACR/wide-area.c
	#avahi-core/iface-none.c avahi-core/iface-pfroute.c avahi-core/avahi-reflector.c
}

HEADERS+= $$PWD/qzeroconfservice.h $$PWD/qzeroconfglobal.h

SOURCES+= $$PWD/qzeroconfservice.cpp
