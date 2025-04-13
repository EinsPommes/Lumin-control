#pragma once

#include "core/rgbcontroller.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QVariantMap>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief Manager für RGB-Profile
 * 
 * Diese Klasse verwaltet das Speichern und Laden von RGB-Profilen.
 * Profile werden als JSON-Dateien im Ordner profiles/ gespeichert.
 */
class ProfileManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param rgbController Pointer auf den RGB-Controller
     * @param parent Parent-Objekt
     */
    explicit ProfileManager(RGBController *rgbController, QObject *parent = nullptr);
    
    /**
     * @brief Destruktor
     */
    ~ProfileManager();
    
    /**
     * @brief Gibt eine Liste aller verfügbaren Profile zurück
     * @return Liste von Profilnamen (ohne Dateiendung)
     */
    QStringList getAvailableProfiles() const;
    
    /**
     * @brief Lädt ein Profil
     * @param profileName Name des Profils (ohne Dateiendung)
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool loadProfile(const QString &profileName);
    
    /**
     * @brief Speichert ein Profil
     * @param profileName Name des Profils (ohne Dateiendung)
     * @param includeTemperatureRules Ob Temperaturregeln gespeichert werden sollen
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool saveProfile(const QString &profileName, bool includeTemperatureRules = true);
    
    /**
     * @brief Löscht ein Profil
     * @param profileName Name des Profils (ohne Dateiendung)
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool deleteProfile(const QString &profileName);
    
    /**
     * @brief Setzt das Standardprofil
     * @param profileName Name des Profils (ohne Dateiendung)
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool setDefaultProfile(const QString &profileName);
    
    /**
     * @brief Gibt den Namen des Standardprofils zurück
     * @return Name des Standardprofils oder leerer String, wenn keines gesetzt ist
     */
    QString getDefaultProfile() const;
    
    /**
     * @brief Lädt das Standardprofil
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool loadDefaultProfile();
    
    /**
     * @brief Prüft, ob ein Profil existiert
     * @param profileName Name des Profils (ohne Dateiendung)
     * @return true wenn das Profil existiert, false wenn nicht
     */
    bool profileExists(const QString &profileName) const;

signals:
    /**
     * @brief Signal, das ausgelöst wird, wenn ein Profil geladen wurde
     * @param profileName Name des Profils
     */
    void profileLoaded(const QString &profileName);
    
    /**
     * @brief Signal, das ausgelöst wird, wenn ein Profil gespeichert wurde
     * @param profileName Name des Profils
     */
    void profileSaved(const QString &profileName);
    
    /**
     * @brief Signal, das ausgelöst wird, wenn ein Profil gelöscht wurde
     * @param profileName Name des Profils
     */
    void profileDeleted(const QString &profileName);
    
    /**
     * @brief Signal, das ausgelöst wird, wenn ein Fehler aufgetreten ist
     * @param errorMessage Fehlermeldung
     */
    void error(const QString &errorMessage);

private:
    /**
     * @brief Erstellt den Profilordner, falls er nicht existiert
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool ensureProfileDirectoryExists();
    
    /**
     * @brief Gibt den Pfad zu einem Profil zurück
     * @param profileName Name des Profils (ohne Dateiendung)
     * @return Vollständiger Pfad zur Profildatei
     */
    QString getProfilePath(const QString &profileName) const;
    
    /**
     * @brief Konvertiert die aktuellen Gerätezustände in ein JSON-Objekt
     * @param includeTemperatureRules Ob Temperaturregeln einbezogen werden sollen
     * @return JSON-Objekt mit den Gerätezuständen
     */
    QJsonObject deviceStatesToJson(bool includeTemperatureRules) const;
    
    /**
     * @brief Wendet ein JSON-Objekt auf die Geräte an
     * @param json JSON-Objekt mit den Gerätezuständen
     * @return true wenn erfolgreich, false wenn fehlgeschlagen
     */
    bool applyJsonToDevices(const QJsonObject &json);

private:
    RGBController *m_rgbController;
    QDir m_profilesDir;
    QString m_defaultProfilePath;
};
