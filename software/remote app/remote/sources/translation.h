#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QTranslator>
#include <QGuiApplication>
#include <QObject>
#include <QQmlEngine>

class TranslationHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)

public:
    explicit TranslationHandler(QQmlEngine *engine)
    {
        m_translator = new QTranslator(this);
        m_engine = engine;
    }

    QString getEmptyString() {
        return "";
    }

    Q_INVOKABLE void selectLanguage(QString language)
    {
        qGuiApp->removeTranslator(m_translator);
        m_translator->load(":/translations/" + language);
        qGuiApp->installTranslator(m_translator);
        //          m_engine->retranslate();
        emit languageChanged();
    }

signals:
    void languageChanged();

private:
    QTranslator *m_translator;
    QQmlEngine *m_engine;

};

#endif // TRANSLATION_H
