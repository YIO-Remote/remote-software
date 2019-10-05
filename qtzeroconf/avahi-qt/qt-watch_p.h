#ifndef QAVAHIP_H
#define QAVAHIP_H

/***
  This file is part of avahi.

  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.

  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

/** \file qt-watch_p.h Qt main loop adapter */

#include <QTimer>
#include <QSocketNotifier>
#include <avahi-common/watch.h>

class AvahiWatch : public QObject
{
    Q_OBJECT
public:
    AvahiWatch(int fd, AvahiWatchEvent event, AvahiWatchCallback callback, void* userdata);
    ~AvahiWatch() {}
    AvahiWatchEvent getEvents() const { return m_incallback ? m_lastEvent : (AvahiWatchEvent)0; }
    void setWatchedEvents(AvahiWatchEvent event);

private slots:
    void gotIn();
    void gotOut();

private:
    QSocketNotifier* m_in;
    QSocketNotifier* m_out;
    //FIXME: ERR and HUP?
    AvahiWatchCallback m_callback;
    AvahiWatchEvent m_lastEvent;
    int m_fd;
    void* m_userdata;
    bool m_incallback;
};

class AvahiTimeout : public QObject
{
    Q_OBJECT

public:
    AvahiTimeout(const struct timeval* tv, AvahiTimeoutCallback callback, void* userdata);
    ~AvahiTimeout() {}
    void update(const struct timeval* tv);

private slots:
    void timeout();

private:
    QTimer m_timer;
    AvahiTimeoutCallback m_callback;
    void* m_userdata;
};

#endif
