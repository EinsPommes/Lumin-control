#include "devices/demorgbdeviceplugin.h"
#include <QDebug>

// DemoRGBDevice Implementation
DemoRGBDevice::DemoRGBDevice(const QString &id, const QString &displayName, const QString &type, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_displayName(displayName)
    , m_type(type)
    , m_currentColor(Qt::white)
    , m_currentEffect("Static")
    , m_connected(true)
{
}

QString DemoRGBDevice::getId() const
{
    return m_id;
}

QString DemoRGBDevice::getDisplayName() const
{
    return m_displayName;
}

QString DemoRGBDevice::getType() const
{
    return m_type;
}

bool DemoRGBDevice::setColor(const QColor &color)
{
    qDebug() << "Demo-Gerät" << m_displayName << "Farbe gesetzt:" << color.name();
    m_currentColor = color;
    return true;
}

bool DemoRGBDevice::setEffect(const QString &effectName, const QVariantMap &parameters)
{
    qDebug() << "Demo-Gerät" << m_displayName << "Effekt gesetzt:" << effectName;
    m_currentEffect = effectName;
    return true;
}

bool DemoRGBDevice::isConnected() const
{
    return m_connected;
}

// DemoRGBDevicePlugin Implementation
DemoRGBDevicePlugin::DemoRGBDevicePlugin(QObject *parent)
    : QObject(parent)
{
}

DemoRGBDevicePlugin::~DemoRGBDevicePlugin()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

bool DemoRGBDevicePlugin::initialize()
{
    qDebug() << "Demo-Plugin wird initialisiert...";
    
    // Demo-Geräte erstellen
    m_devices.append(new DemoRGBDevice("demo_keyboard", "Demo RGB Tastatur", "Keyboard", this));
    m_devices.append(new DemoRGBDevice("demo_mouse", "Demo RGB Maus", "Mouse", this));
    m_devices.append(new DemoRGBDevice("demo_headset", "Demo RGB Headset", "Headset", this));
    m_devices.append(new DemoRGBDevice("demo_strip", "Demo RGB LED-Streifen", "LED Strip", this));
    m_devices.append(new DemoRGBDevice("demo_fan", "Demo RGB Lüfter", "Fan", this));
    
    qDebug() << "Demo-Plugin initialisiert mit" << m_devices.size() << "Geräten";
    return true;
}

QString DemoRGBDevicePlugin::getName() const
{
    return "Demo RGB Device Plugin";
}

QString DemoRGBDevicePlugin::getVersion() const
{
    return "1.0.0";
}

QList<IRGBDevice*> DemoRGBDevicePlugin::getDevices()
{
    QList<IRGBDevice*> devices;
    for (auto device : m_devices) {
        devices.append(device);
    }
    return devices;
}
