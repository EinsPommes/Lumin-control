#pragma once

#include "irgbdevice.h"
#include <QtPlugin>
#include <QString>
#include <QList>

/**
 * @brief Interface für RGB-Geräteplugins
 * 
 * Dieses Interface definiert die grundlegenden Funktionen, die jedes Plugin
 * implementieren muss, um mit LuminControl kompatibel zu sein.
 */
class IRGBDevicePlugin {
public:
    virtual ~IRGBDevicePlugin() = default;

    /**
     * @brief Initialisiert das Plugin
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    virtual bool initialize() = 0;

    /**
     * @brief Gibt den Namen des Plugins zurück
     * @return Name des Plugins
     */
    virtual QString getName() const = 0;

    /**
     * @brief Gibt die Version des Plugins zurück
     * @return Version des Plugins
     */
    virtual QString getVersion() const = 0;

    /**
     * @brief Gibt eine Liste aller vom Plugin unterstützten Geräte zurück
     * @return Liste von IRGBDevice-Objekten
     */
    virtual QList<IRGBDevice*> getDevices() = 0;
};

Q_DECLARE_INTERFACE(IRGBDevicePlugin, "org.lumincontrol.IRGBDevicePlugin")
