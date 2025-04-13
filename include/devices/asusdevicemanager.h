#pragma once

#include "devices/asusrgbdevice.h"
#include <QObject>
#include <QList>

/**
 * @brief Manager für ASUS RGB-Geräte
 * 
 * Diese Klasse verwaltet ASUS RGB-Geräte und integriert sie in das DeviceManager-System.
 */
class AsusDeviceManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param parent Parent-QObject
     */
    explicit AsusDeviceManager(QObject *parent = nullptr);

    /**
     * @brief Destruktor
     */
    ~AsusDeviceManager();

    /**
     * @brief Initialisiert den ASUS-Gerätemanager
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool initialize();

    /**
     * @brief Gibt eine Liste aller ASUS RGB-Geräte zurück
     * @return Liste von AsusRGBDevice-Objekten
     */
    QList<AsusRGBDevice*> getDevices() const;

private:
    /**
     * @brief Sucht nach ASUS RGB-Geräten
     */
    void scanForDevices();

    /**
     * @brief Entfernt alle Geräte
     */
    void clearDevices();

private:
    bool m_initialized;
    QList<AsusRGBDevice*> m_devices;
};
