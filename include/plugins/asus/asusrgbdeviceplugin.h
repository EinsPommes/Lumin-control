#pragma once

#include "devices/irgbdevice.h"
#include "devices/irgbdeviceplugin.h"
#include <QObject>
#include <QColor>
#include <QStringList>
#include <QVariantMap>

// Wenn die ASUS Aura SDK verfügbar ist, definieren wir USE_ASUS_SDK
// Andernfalls verwenden wir eine Simulation für Testzwecke
//#define USE_ASUS_SDK

/**
 * @brief Repräsentiert ein ASUS RGB-Gerät
 * 
 * Diese Klasse implementiert das IRGBDevice-Interface für ASUS RGB-Geräte.
 * Sie kann entweder mit der ASUS Aura SDK oder im Simulationsmodus arbeiten.
 */
class AsusRGBDevice : public QObject, public IRGBDevice
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param id Eindeutige ID des Geräts
     * @param name Anzeigename des Geräts
     * @param type Typ des Geräts (z.B. Motherboard, GPU, etc.)
     * @param parent Parent-QObject
     */
    AsusRGBDevice(const QString &id, const QString &name, const QString &type, QObject *parent = nullptr);

    /**
     * @brief Destruktor
     */
    ~AsusRGBDevice() override;

    // IRGBDevice-Interface-Implementierung
    QString getId() const override;
    QString getDisplayName() const override;
    QString getType() const override;
    QStringList getSupportedEffects() const;
    bool setColor(const QColor &color) override;
    bool setEffect(const QString &effectName, const QVariantMap &params = QVariantMap()) override;
    QColor getColor() const override;
    QString getActiveEffect() const override;
    QVariantMap getEffectParameters() const override;
    bool isConnected() const override;

private:
    QString m_id;
    QString m_name;
    QString m_type;
    QColor m_currentColor;
    QString m_currentEffect;
    QVariantMap m_currentParameters;
    QStringList m_supportedEffects;
};

/**
 * @brief Plugin für ASUS RGB-Geräte
 * 
 * Dieses Plugin implementiert das IRGBDevicePlugin-Interface für ASUS RGB-Geräte.
 * Es kann entweder mit der ASUS Aura SDK oder im Simulationsmodus arbeiten.
 */
class AsusRGBDevicePlugin : public QObject, public IRGBDevicePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.lumincontrol.IRGBDevicePlugin" FILE "asusrgbdeviceplugin.json")
    Q_INTERFACES(IRGBDevicePlugin)

public:
    /**
     * @brief Konstruktor
     * @param parent Parent-QObject
     */
    AsusRGBDevicePlugin(QObject *parent = nullptr);

    /**
     * @brief Destruktor
     */
    ~AsusRGBDevicePlugin() override;

    // IRGBDevicePlugin-Interface-Implementierung
    bool initialize() override;
    QString getName() const override;
    QString getVersion() const override;
    QString getAuthor() const override;
    QList<IRGBDevice*> getDevices() const override;

private:
    /**
     * @brief Herunterfahren des Plugins
     */
    void shutdown();

    /**
     * @brief Sucht nach ASUS RGB-Geräten
     */
    void scanForDevices();

    /**
     * @brief Entfernt alle Geräte
     */
    void clearDevices();

    /**
     * @brief Initialisiert die ASUS Aura SDK
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool initializeAuraSDK();

    /**
     * @brief Fährt die ASUS Aura SDK herunter
     */
    void shutdownAuraSDK();

private:
    bool m_initialized;
    QList<AsusRGBDevice*> m_devices;
};
