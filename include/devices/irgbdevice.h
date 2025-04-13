#pragma once

#include <QString>
#include <QColor>
#include <QVariantMap>

/**
 * @brief Interface für RGB-Geräte
 * 
 * Dieses Interface definiert die grundlegenden Funktionen, die jedes RGB-Gerät
 * implementieren muss, um mit LuminControl kompatibel zu sein.
 */
class IRGBDevice {
public:
    virtual ~IRGBDevice() = default;

    /**
     * @brief Gibt die eindeutige ID des Geräts zurück
     * @return Eindeutige Geräte-ID
     */
    virtual QString getId() const = 0;

    /**
     * @brief Gibt den Anzeigenamen des Geräts zurück
     * @return Anzeigename für die UI
     */
    virtual QString getDisplayName() const = 0;

    /**
     * @brief Gibt den Typ des Geräts zurück (z.B. Tastatur, Maus, Lüfter)
     * @return Gerätetyp
     */
    virtual QString getType() const = 0;

    /**
     * @brief Setzt die Farbe des Geräts
     * @param color Die zu setzende Farbe
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    virtual bool setColor(const QColor &color) = 0;
    
    /**
     * @brief Gibt die aktuelle Farbe des Geräts zurück
     * @return Aktuelle Farbe
     */
    virtual QColor getColor() const = 0;

    /**
     * @brief Setzt einen Effekt für das Gerät
     * @param effectName Name des Effekts
     * @param parameters Optionale Parameter für den Effekt
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    virtual bool setEffect(const QString &effectName, const QVariantMap &parameters = QVariantMap()) = 0;
    
    /**
     * @brief Gibt den Namen des aktuellen Effekts zurück
     * @return Name des aktuellen Effekts
     */
    virtual QString getActiveEffect() const = 0;
    
    /**
     * @brief Gibt die Parameter des aktuellen Effekts zurück
     * @return Parameter des aktuellen Effekts
     */
    virtual QVariantMap getEffectParameters() const = 0;

    /**
     * @brief Prüft, ob das Gerät verbunden ist
     * @return true wenn verbunden, false wenn nicht
     */
    virtual bool isConnected() const = 0;
};
