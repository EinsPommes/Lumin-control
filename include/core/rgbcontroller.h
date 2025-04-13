#pragma once

#include "core/color.h"
#include "core/effect.h"
#include "devices/irgbdevice.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QColor>
#include <QTimer>
#include <QSignalMapper>

/**
 * @brief Controller für RGB-Geräte
 * 
 * Diese Klasse verwaltet die RGB-Geräte und deren Effekte.
 * Sie dient als Schnittstelle zwischen der GUI und den Geräten.
 */
class RGBController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param parent Parent-Objekt
     */
    explicit RGBController(QObject *parent = nullptr);
    
    /**
     * @brief Destruktor
     */
    ~RGBController();
    
    /**
     * @brief Registriert ein Gerät
     * @param device Gerät
     */
    void registerDevice(IRGBDevice *device);
    
    /**
     * @brief Entfernt ein Gerät
     * @param device Gerät
     */
    void unregisterDevice(IRGBDevice *device);
    
    /**
     * @brief Gibt eine Liste aller registrierten Geräte zurück
     * @return Liste von Geräten
     */
    QList<IRGBDevice*> getDevices() const;
    
    /**
     * @brief Sucht ein Gerät anhand seiner ID
     * @param id ID des Geräts
     * @return Gerät oder nullptr, wenn nicht gefunden
     */
    IRGBDevice* getDeviceById(const QString &id) const;
    
    /**
     * @brief Setzt die Farbe für ein Gerät
     * @param device Gerät
     * @param color Farbe
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool setColorForDevice(IRGBDevice *device, const QColor &color);
    
    /**
     * @brief Setzt die Farbe für mehrere Geräte
     * @param devices Liste von Geräten
     * @param color Farbe
     * @return Anzahl der Geräte, bei denen die Farbe erfolgreich gesetzt wurde
     */
    int setColorForDevices(const QList<IRGBDevice*> &devices, const QColor &color);
    
    /**
     * @brief Setzt die Farbe für alle Geräte
     * @param color Farbe
     * @return Anzahl der Geräte, bei denen die Farbe erfolgreich gesetzt wurde
     */
    int setColorForAllDevices(const QColor &color);
    
    /**
     * @brief Setzt einen Effekt für ein Gerät
     * @param device Gerät
     * @param effectName Name des Effekts
     * @param parameters Parameter für den Effekt
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool setEffectForDevice(IRGBDevice *device, const QString &effectName, const QVariantMap &parameters = QVariantMap());
    
    /**
     * @brief Setzt einen Effekt für mehrere Geräte
     * @param devices Liste von Geräten
     * @param effectName Name des Effekts
     * @param parameters Parameter für den Effekt
     * @return Anzahl der Geräte, bei denen der Effekt erfolgreich gesetzt wurde
     */
    int setEffectForDevices(const QList<IRGBDevice*> &devices, const QString &effectName, const QVariantMap &parameters = QVariantMap());
    
    /**
     * @brief Setzt einen Effekt für alle Geräte
     * @param effectName Name des Effekts
     * @param parameters Parameter für den Effekt
     * @return Anzahl der Geräte, bei denen der Effekt erfolgreich gesetzt wurde
     */
    int setEffectForAllDevices(const QString &effectName, const QVariantMap &parameters = QVariantMap());
    
    /**
     * @brief Erstellt einen Effekt
     * @param effectName Name des Effekts
     * @param parameters Parameter für den Effekt
     * @return Pointer auf den erstellten Effekt
     */
    Effect* createEffect(const QString &effectName, const QVariantMap &parameters = QVariantMap());
    
    /**
     * @brief Gibt eine Liste aller verfügbaren Effektnamen zurück
     * @return Liste von Effektnamen
     */
    QStringList getAvailableEffects() const;
    
    /**
     * @brief Koppelt die RGB-Farbe an die Temperatur
     * @param enabled true um zu aktivieren, false um zu deaktivieren
     */
    void setTemperatureLinking(bool enabled);
    
    /**
     * @brief Prüft, ob die RGB-Farbe an die Temperatur gekoppelt ist
     * @return true wenn aktiviert, false wenn deaktiviert
     */
    bool isTemperatureLinkingEnabled() const;
    
    /**
     * @brief Aktualisiert die Temperaturwerte
     * @param cpuTemp CPU-Temperatur (0-100)
     * @param gpuTemp GPU-Temperatur (0-100)
     */
    void updateTemperatures(int cpuTemp, int gpuTemp);

signals:
    /**
     * @brief Signal, das bei Farbänderung ausgelöst wird
     * @param deviceId ID des Geräts
     * @param color Die neue Farbe
     */
    void colorChanged(const QString &deviceId, const QColor &color);
    
    /**
     * @brief Signal, das bei Effektänderung ausgelöst wird
     * @param deviceId ID des Geräts
     * @param effectName Name des Effekts
     */
    void effectChanged(const QString &deviceId, const QString &effectName);
    
    /**
     * @brief Signal, das ausgelöst wird, wenn eine Aktion erfolgreich war
     * @param message Erfolgsmeldung
     */
    void actionSuccess(const QString &message);
    
    /**
     * @brief Signal, das ausgelöst wird, wenn eine Aktion fehlgeschlagen ist
     * @param message Fehlermeldung
     */
    void actionError(const QString &message);

private slots:
    /**
     * @brief Aktualisiert die Farbe für ein Gerät basierend auf dem aktiven Effekt
     * @param deviceId ID des Geräts
     */
    void updateDeviceColor(const QString &deviceId);

private:
    QList<IRGBDevice*> m_devices;
    QMap<QString, Effect*> m_deviceEffects;
    QSignalMapper *m_effectMapper;
    bool m_temperatureLinkingEnabled;
    int m_cpuTemperature;
    int m_gpuTemperature;
};
