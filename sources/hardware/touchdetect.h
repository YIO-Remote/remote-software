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
