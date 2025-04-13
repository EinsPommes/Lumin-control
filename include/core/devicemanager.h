#pragma once

#include "devices/irgbdeviceplugin.h"
#include "devices/irgbdevice.h"
#include "devices/asusdevicemanager.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QPluginLoader>
#include <QDir>
#include <memory>

/**
 * @brief Manager für RGB-Geräte und Plugins
 * 
 * Diese Klasse ist für das Laden und Verwalten von Plugins und deren Geräten zuständig.
 * Sie lädt Plugins aus dem plugins/-Ordner und stellt Methoden bereit, um auf die
 * geladenen Geräte zuzugreifen.
 */
class DeviceManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param parent Parent-Objekt
     */
    explicit DeviceManager(QObject *parent = nullptr);
    
    /**
     * @brief Destruktor
     */
    ~DeviceManager();

    /**
     * @brief Lädt alle Plugins aus dem plugins/-Ordner
     * @return Anzahl der erfolgreich geladenen Plugins
     */
    int loadPlugins();

    /**
     * @brief Gibt eine Liste aller geladenen Plugins zurück
     * @return Liste von Plugin-Pointern
     */
    QList<IRGBDevicePlugin*> getPlugins() const;

    /**
     * @brief Gibt eine Liste aller verfügbaren Geräte zurück
     * @return Liste von Geräte-Pointern
     */
    QList<IRGBDevice*> getAllDevices() const;

    /**
     * @brief Sucht ein Gerät anhand seiner ID
     * @param id ID des gesuchten Geräts
     * @return Pointer auf das Gerät oder nullptr wenn nicht gefunden
     */
    IRGBDevice* getDeviceById(const QString &id) const;

    /**
     * @brief Setzt die Farbe für alle Geräte
     * @param color Die zu setzende Farbe
     * @return Anzahl der Geräte, bei denen die Farbe erfolgreich gesetzt wurde
     */
    int setColorForAllDevices(const QColor &color);

    /**
     * @brief Setzt einen Effekt für alle Geräte
     * @param effectName Name des Effekts
     * @param parameters Optionale Parameter für den Effekt
     * @return Anzahl der Geräte, bei denen der Effekt erfolgreich gesetzt wurde
     */
    int setEffectForAllDevices(const QString &effectName, const QVariantMap &parameters = QVariantMap());

signals:
    /**
     * @brief Signal, das ausgelöst wird, wenn ein neues Plugin geladen wurde
     * @param plugin Das geladene Plugin
     */
    void pluginLoaded(IRGBDevicePlugin *plugin);

    /**
     * @brief Signal, das ausgelöst wird, wenn ein neues Gerät erkannt wurde
     * @param device Das erkannte Gerät
     */
    void deviceDiscovered(IRGBDevice *device);

private:
    QList<QPluginLoader*> m_pluginLoaders;
    QList<IRGBDevicePlugin*> m_plugins;
    QMap<QString, IRGBDevice*> m_devices;
    
    // ASUS-Gerätemanager für integrierte ASUS-Unterstützung
    AsusDeviceManager* m_asusManager;
};
