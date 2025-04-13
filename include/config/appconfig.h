#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

/**
 * @brief Klasse für Anwendungskonfiguration
 * 
 * Diese Klasse verwaltet die Anwendungskonfiguration wie Thema, Startverhalten usw.
 */
class AppConfig : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param parent Parent-Objekt
     */
    explicit AppConfig(QObject *parent = nullptr);
    
    /**
     * @brief Destruktor
     */
    ~AppConfig();
    
    /**
     * @brief Setzt das Anwendungsthema
     * @param themeName Name des Themes
     */
    void setTheme(const QString &themeName);
    
    /**
     * @brief Gibt das aktuelle Anwendungsthema zurück
     * @return Name des Themes
     */
    QString getTheme() const;
    
    /**
     * @brief Setzt, ob die Anwendung mit dem System starten soll
     * @param enabled true um zu aktivieren, false um zu deaktivieren
     */
    void setStartWithSystem(bool enabled);
    
    /**
     * @brief Prüft, ob die Anwendung mit dem System startet
     * @return true wenn aktiviert, false wenn deaktiviert
     */
    bool startsWithSystem() const;
    
    /**
     * @brief Speichert die Konfiguration
     */
    void saveConfig();
    
    /**
     * @brief Lädt die Konfiguration
     */
    void loadConfig();

private:
    QSettings m_settings;
    QString m_theme;
    bool m_startWithSystem;
};
