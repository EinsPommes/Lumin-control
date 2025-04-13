#include "devices/asusdevicemanager.h"
#include <QDebug>

AsusDeviceManager::AsusDeviceManager(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    qDebug() << "AsusDeviceManager konstruiert";
}

AsusDeviceManager::~AsusDeviceManager()
{
    clearDevices();
}

bool AsusDeviceManager::initialize()
{
    if (m_initialized) {
        return true;
    }
    
    qDebug() << "Initialisiere ASUS Device Manager";
    
    scanForDevices();
    m_initialized = true;
    
    qDebug() << "ASUS Device Manager initialisiert mit" << m_devices.size() << "Geräten";
    
    return true;
}

QList<AsusRGBDevice*> AsusDeviceManager::getDevices() const
{
    return m_devices;
}

void AsusDeviceManager::scanForDevices()
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
    
    AsusRGBDevice *ram = new AsusRGBDevice(
        "asus-ram-1",
        "ASUS ROG Strix RGB DDR5",
        "RAM",
        this
    );
    m_devices.append(ram);
    
    AsusRGBDevice *keyboard = new AsusRGBDevice(
        "asus-kb-1",
        "ASUS ROG Strix Scope NX",
        "Keyboard",
        this
    );
    m_devices.append(keyboard);
    
    AsusRGBDevice *mouse = new AsusRGBDevice(
        "asus-mouse-1",
        "ASUS ROG Gladius III",
        "Mouse",
        this
    );
    m_devices.append(mouse);
}

void AsusDeviceManager::clearDevices()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}
