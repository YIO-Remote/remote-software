/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#ifndef TOUCHDETECT_H
#define TOUCHDETECT_H

#pragma once

#include <QQuickItem>

class TouchEventFilter : public QQuickItem
{
    Q_OBJECT
public:
    Q_PROPERTY(QObject * source READ getSource WRITE setSource)
    Q_PROPERTY(bool detected NOTIFY detectedChanged)

public:
    TouchEventFilter()
    {
        m_source = nullptr;
    }

    ~TouchEventFilter()
    {
        if (m_source != nullptr)
            m_source->removeEventFilter(this);
    }

    void setSource(QObject *source)
    {
        source->installEventFilter(this);
        m_source = source;
    }

    QObject * getSource() { return m_source; }


private:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        Q_UNUSED(obj)

        switch (event->type()) {
        case QEvent::TouchBegin:
        case QEvent::MouseButtonPress:
            emit detectedChanged();
            break;
        default:  // do nothing
            break;
        }
        return QQuickItem::eventFilter(this, event);

    }

signals:
    void detectedChanged();

private:
    QObject *m_source;

};

#endif // TOUCHDETECT_H
