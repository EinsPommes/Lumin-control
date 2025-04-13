#include "devices/asusrgbdevice.h"
#include <QDebug>

AsusRGBDevice::AsusRGBDevice(const QString &id, const QString &name, const QString &type, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_name(name)
    , m_type(type)
    , m_currentColor(Qt::black)
    , m_currentEffect("Static")
{
    // Unterstützte Effekte hinzufügen
    m_supportedEffects << "Static" << "Breathing" << "ColorCycle" << "Rainbow" << "Strobe";
}

AsusRGBDevice::~AsusRGBDevice()
{
    // Nichts zu tun
}

QString AsusRGBDevice::getId() const
{
    return m_id;
}

QString AsusRGBDevice::getDisplayName() const
{
    return m_name;
}

QString AsusRGBDevice::getType() const
{
    return m_type;
}

QStringList AsusRGBDevice::getSupportedEffects() const
{
    return m_supportedEffects;
}

bool AsusRGBDevice::setColor(const QColor &color)
{
    qDebug() << "Setze Farbe für ASUS-Gerät" << m_name << "auf" << color.name();
    
    m_currentColor = color;
    m_currentEffect = "Static"; // Wenn eine Farbe gesetzt wird, wechseln wir zum statischen Effekt
    
    return true;
}

bool AsusRGBDevice::setEffect(const QString &effectName, const QVariantMap &params)
{
    if (!m_supportedEffects.contains(effectName)) {
        qWarning() << "Effekt" << effectName << "wird nicht vom ASUS-Gerät" << m_name << "unterstützt";
        return false;
    }
    
    qDebug() << "Setze Effekt für ASUS-Gerät" << m_name << "auf" << effectName << "mit Parametern" << params;
    
    m_currentEffect = effectName;
    
    // Wenn der Effekt "Static" ist und eine Farbe in den Parametern angegeben ist,
    // aktualisieren wir die aktuelle Farbe
    if (effectName == "Static" && params.contains("color")) {
        m_currentColor = params.value("color").value<QColor>();
    }
    
    // Parameter speichern
    m_currentParameters = params;
    
    return true;
}

QColor AsusRGBDevice::getColor() const
{
    return m_currentColor;
}

QString AsusRGBDevice::getActiveEffect() const
{
    return m_currentEffect;
}

QVariantMap AsusRGBDevice::getEffectParameters() const
{
    return m_currentParameters;
}

bool AsusRGBDevice::isConnected() const
{
    // Für die Simulation geben wir immer true zurück
    return true;
}
