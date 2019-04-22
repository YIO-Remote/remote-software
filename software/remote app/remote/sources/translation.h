#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QTranslator>
#include <QGuiApplication>
#include <QObject>
#include <QQmlEngine>

class TranslationHandler : public QObject
{
    Q_OBJECT
public:
    explicit TranslationHandler(QQmlEngine *engine)
    {
        m_translator = new QTranslator(this);
        m_engine = engine;
    }

    Q_INVOKABLE void selectLanguage(QString language)
    {
          qGuiApp->removeTranslator(m_translator);
          m_translator->load(":/translations/" + language);
          qGuiApp->installTranslator(m_translator);
          m_engine->retranslate();
    }

private:
    QTranslator *m_translator;
    QQmlEngine *m_engine;

};

#endif // TRANSLATION_H
