#include "plugins/asus/asusrgbdeviceplugin.h"
#include <QDebug>

// AsusRGBDevicePlugin-Implementierung

AsusRGBDevicePlugin::AsusRGBDevicePlugin(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    qDebug() << "AsusRGBDevicePlugin konstruiert";
}

AsusRGBDevicePlugin::~AsusRGBDevicePlugin()
{
    shutdown();
}

bool AsusRGBDevicePlugin::initialize()
{
    if (m_initialized) {
        return true;
    }
    
    qDebug() << "Initialisiere ASUS RGB Device Plugin";
    
    // Simulierte Geräte für Testzwecke
    AsusRGBDevice *motherboard = new AsusRGBDevice(
        "asus-mb-1",
        "ASUS ROG Strix Z690-E Gaming WiFi",
        "Motherboard",
        this
    );
    m_devices.append(motherboard);
    
    AsusRGBDevice *gpu = new AsusRGBDevice(
        "asus-gpu-1",
        "ASUS ROG Strix GeForce RTX 3080",
        "GPU",
        this
    );
    m_devices.append(gpu);
    
    m_initialized = true;
    qDebug() << "ASUS RGB Device Plugin initialisiert mit" << m_devices.size() << "Geräten";
    
    return true;
}

void AsusRGBDevicePlugin::shutdown()
{
    if (!m_initialized) {
        return;
    }
    
    qDebug() << "Fahre ASUS RGB Device Plugin herunter";
    
    qDeleteAll(m_devices);
    m_devices.clear();
    
    m_initialized = false;
}

void AsusRGBDevicePlugin::scanForDevices()
{
    // Zuerst vorhandene Geräte entfernen
    clearDevices();
    
    // Simulierte Geräte für Testzwecke
    AsusRGBDevice *motherboard = new AsusRGBDevice(
        "asus-mb-1",
        "ASUS ROG Strix Z690-E Gaming WiFi",
        "Motherboard",
        this
    );
    m_devices.append(motherboard);
    
    AsusRGBDevice *gpu = new AsusRGBDevice(
        "asus-gpu-1",
        "ASUS ROG Strix GeForce RTX 3080",
        "GPU",
        this
    );
    m_devices.append(gpu);
}

void AsusRGBDevicePlugin::clearDevices()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

bool AsusRGBDevicePlugin::initializeAuraSDK()
{
    // Simulation für Testzwecke
    return true;
}

void AsusRGBDevicePlugin::shutdownAuraSDK()
{
    // Nichts zu tun in der Simulation
}

QString AsusRGBDevicePlugin::getName() const
{
    return "ASUS Aura Sync";
}

QString AsusRGBDevicePlugin::getVersion() const
{
    return "1.0.0";
}

QString AsusRGBDevicePlugin::getAuthor() const
{
    return "LuminControl Team";
}

QList<IRGBDevice*> AsusRGBDevicePlugin::getDevices() const
{
    QList<IRGBDevice*> devices;
    for (AsusRGBDevice *device : m_devices) {
        devices.append(device);
    }
    return devices;
}

// AsusRGBDevice-Implementierung

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
