#include "core/devicemanager.h"
#include <QApplication>
#include <QDebug>
#include <QDir>

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
    , m_asusManager(nullptr)
{
    // ASUS-Gerätemanager erstellen und initialisieren
    m_asusManager = new AsusDeviceManager(this);
    
    // Plugins beim Start laden
    loadPlugins();
}

DeviceManager::~DeviceManager()
{
    // Plugins freigeben
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
    
    // Vorhandene Plugins freigeben
    for (auto loader : m_pluginLoaders) {
        loader->unload();
        delete loader;
    }
    m_pluginLoaders.clear();
    m_plugins.clear();
    m_devices.clear();
    
    // Nach Plugins suchen und laden
    int loadedPlugins = 0;
    
    // Dynamische Plugins laden
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (fileName.endsWith(".dll") || fileName.endsWith(".so") || fileName.endsWith(".dylib")) {
            QPluginLoader *loader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader->instance();
            
            if (plugin) {
                IRGBDevicePlugin *devicePlugin = qobject_cast<IRGBDevicePlugin*>(plugin);
                if (devicePlugin) {
                    qDebug() << "Plugin gefunden:" << devicePlugin->getName() << "von" << devicePlugin->getAuthor();
                    
                    // Plugin initialisieren
                    if (devicePlugin->initialize()) {
                        m_plugins.append(devicePlugin);
                        m_pluginLoaders.append(loader);
                        
                        // Geräte des Plugins laden
                        QList<IRGBDevice*> devices = devicePlugin->getDevices();
                        for (IRGBDevice *device : devices) {
                            QString deviceId = device->getId();
                            m_devices.insert(deviceId, device);
                            qDebug() << "Gerät gefunden:" << device->getDisplayName() << "(" << device->getType() << ")";
                            
                            // Signal für neu erkanntes Gerät senden
                            emit deviceDiscovered(device);
                        }
                        
                        loadedPlugins++;
                    } else {
                        qWarning() << "Plugin konnte nicht initialisiert werden:" << devicePlugin->getName();
                        loader->unload();
                        delete loader;
                    }
                } else {
                    qWarning() << "Datei ist kein gültiges RGB-Geräteplugin:" << fileName;
                    loader->unload();
                    delete loader;
                }
            } else {
                qWarning() << "Fehler beim Laden des Plugins:" << fileName << "-" << loader->errorString();
                delete loader;
            }
        }
    }
    
    // ASUS-Gerätemanager initialisieren
    if (m_asusManager && m_asusManager->initialize()) {
        qDebug() << "ASUS-Gerätemanager erfolgreich initialisiert";
        
        // ASUS-Geräte in die Geräteliste einfügen
        QList<AsusRGBDevice*> asusDevices = m_asusManager->getDevices();
        for (AsusRGBDevice *device : asusDevices) {
            QString deviceId = device->getId();
            m_devices.insert(deviceId, device);
            qDebug() << "ASUS-Gerät gefunden:" << device->getDisplayName() << "(" << device->getType() << ")";
            
            // Signal für neu erkanntes Gerät senden
            emit deviceDiscovered(device);
        }
    } else {
        qWarning() << "ASUS-Gerätemanager konnte nicht initialisiert werden";
    }
    
    qDebug() << "System bereit für RGB-Geräte";
    qDebug() << loadedPlugins << "Plugins geladen mit" << m_devices.size() << "Geräten";
    
    return loadedPlugins;
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
