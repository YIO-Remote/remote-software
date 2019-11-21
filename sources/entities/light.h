#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QMetaEnum>
#include "entity.h"
#include "lightinterface.h"

class Light : public Entity, LightInterface
{
    Q_OBJECT
    Q_INTERFACES(LightInterface)

public:
    Q_PROPERTY  (bool           state       READ    state       NOTIFY stateChanged)
    Q_PROPERTY  (int            brightness  READ    brightness  NOTIFY brightnessChanged)
    Q_PROPERTY  (QColor         color       READ    color       NOTIFY colorChanged)
    Q_PROPERTY  (int            colorTemp   READ    colorTemp   NOTIFY colorTempChanged)

    // update an entity's attributes
    Q_INVOKABLE bool            update              (const QVariantMap& attributes) override;
    Q_INVOKABLE bool            updateAttrByName    (const QString& name, const QVariant& value) override;
    Q_INVOKABLE bool            updateAttrByIndex   (int attrIndex, const QVariant& value) override;

    // attribute name and index
    Q_INVOKABLE QString         getAttrName         (int attrIndex) override;
    Q_INVOKABLE int             getAttrIndex        (const QString& attrName) override;

    // light commands
    // called from ui like: obj.toggle()
    Q_INVOKABLE void            toggle();
    Q_INVOKABLE void            turnOn();
    Q_INVOKABLE void            turnOff();
    Q_INVOKABLE void            setBrightness(int value);
    Q_INVOKABLE void            setColor(QColor value);
    Q_INVOKABLE void            setColorTemp(int value);

    // only for C++ integrations
    virtual     void*           getSpecificInterface() override;

    explicit Light(const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

    bool                        state() override        { return m_state; }
    int                         brightness() override   { return m_brightness; }
    QColor                      color() override        { return m_color; }
    int                         colorTemp() override    { return m_colorTemp; }

signals:
    void stateChanged();
    void brightnessChanged();
    void colorChanged();
    void colorTempChanged();

public:
    static QString Type;

    static QStringList&         AllFeatures()
    {
        static QStringList s;
        if (s.count() == 0)
            s.append(allFeatures());
        return s;
    }

private:
    static QStringList          allFeatures     ();
    static QMetaEnum            s_metaEnum;

    bool                        m_state;
    QColor                      m_color;
    int                         m_brightness;
    int                         m_colorTemp;
};

#endif // LIGHT_H
