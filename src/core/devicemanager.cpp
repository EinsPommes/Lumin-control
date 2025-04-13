#include "core/devicemanager.h"
#include <QApplication>
#include <QDebug>
#include <QDir>

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
{
    // Direkt beim Start initialisieren
    loadPlugins();
}

DeviceManager::~DeviceManager()
{
    // Plugins und Loader freigeben
    for (auto loader : m_pluginLoaders) {
        loader->unload();
        delete loader;
    }
    
    m_pluginLoaders.clear();
    m_plugins.clear();
    m_devices.clear();
}

int DeviceManager::loadPlugins()
{
    qDebug() << "LuminControl wird initialisiert...";
    
    // Plugins-Verzeichnis bestimmen
    QDir pluginsDir(qApp->applicationDirPath());
    
    // Zuerst im Unterordner "plugins" suchen
    if (pluginsDir.cd("plugins")) {
        qDebug() << "Suche Plugins in:" << pluginsDir.absolutePath();
    } else {
        // Wenn nicht vorhanden, im Entwicklungsmodus nach dem Plugins-Verzeichnis suchen
        pluginsDir = QDir(QApplication::applicationDirPath());
        if (pluginsDir.cdUp() && pluginsDir.cd("plugins")) {
            qDebug() << "Suche Plugins in (Entwicklungsmodus):" << pluginsDir.absolutePath();
        } else {
            qWarning() << "Plugins-Verzeichnis nicht gefunden! Erstelle Verzeichnis...";
            QDir appDir(QApplication::applicationDirPath());
            appDir.mkdir("plugins");
        }
    }
    
    qDebug() << "System bereit für RGB-Geräte";
    
    return 0; // Keine Plugins geladen, aber erfolgreich initialisiert
}

QList<IRGBDevicePlugin*> DeviceManager::getPlugins() const
{
    return m_plugins;
}

QList<IRGBDevice*> DeviceManager::getAllDevices() const
{
    return m_devices.values();
}

IRGBDevice* DeviceManager::getDeviceById(const QString &id) const
{
    return m_devices.value(id, nullptr);
}

int DeviceManager::setColorForAllDevices(const QColor &color)
{
    int successCount = 0;
    
    for (IRGBDevice *device : m_devices.values()) {
        if (device && device->isConnected()) {
            if (device->setColor(color)) {
                successCount++;
            }
        }
    }
    
    return successCount;
}

int DeviceManager::setEffectForAllDevices(const QString &effectName, const QVariantMap &parameters)
{
    int successCount = 0;
    
    for (IRGBDevice *device : m_devices.values()) {
        if (device && device->isConnected()) {
            if (device->setEffect(effectName, parameters)) {
                successCount++;
            }
        }
    }
    
    return successCount;
}
