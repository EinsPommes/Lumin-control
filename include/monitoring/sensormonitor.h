#pragma once

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QMap>
#include <QVector>

#ifdef Q_OS_WIN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#ifndef __MINGW32__
#include <wbemidl.h>
#endif
#endif

/**
 * @brief Die SensorMonitor-Klasse überwacht Systemsensoren wie CPU- und GPU-Temperatur und Auslastung.
 * 
 * Verwendet WMI (Windows Management Instrumentation) auf Windows-Systemen, um Sensordaten abzufragen.
 * Auf anderen Plattformen werden Fallback-Mechanismen oder Simulationsdaten verwendet.
 */
class SensorMonitor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor für den SensorMonitor
     * @param updateInterval Aktualisierungsintervall in Millisekunden
     * @param parent Elternobjekt
     */
    explicit SensorMonitor(int updateInterval = 1000, QObject *parent = nullptr);
    
    /**
     * @brief Destruktor
     */
    ~SensorMonitor();
    
    /**
     * @brief Startet die Sensorüberwachung
     */
    void startMonitoring();
    
    /**
     * @brief Stoppt die Sensorüberwachung
     */
    void stopMonitoring();
    
    /**
     * @brief Liest die aktuelle CPU-Temperatur aus
     * @return CPU-Temperatur in Grad Celsius
     */
    int readCpuTemperature();
    
    /**
     * @brief Liest die aktuelle CPU-Auslastung aus
     * @return CPU-Auslastung in Prozent (0-100)
     */
    int readCpuUsage();
    
    /**
     * @brief Liest die aktuelle GPU-Temperatur aus
     * @return GPU-Temperatur in Grad Celsius
     */
    int readGpuTemperature();
    
    /**
     * @brief Liest die aktuelle GPU-Auslastung aus
     * @return GPU-Auslastung in Prozent (0-100)
     */
    int readGpuUsage();
    
    /**
     * @brief Gibt die letzte gemessene CPU-Temperatur zurück
     * @return CPU-Temperatur in Grad Celsius
     */
    int getCpuTemperature() const { return m_cpuTemperature; }
    
    /**
     * @brief Gibt die letzte gemessene CPU-Auslastung zurück
     * @return CPU-Auslastung in Prozent (0-100)
     */
    int getCpuUsage() const { return m_cpuUsage; }
    
    /**
     * @brief Gibt die letzte gemessene GPU-Temperatur zurück
     * @return GPU-Temperatur in Grad Celsius
     */
    int getGpuTemperature() const { return m_gpuTemperature; }
    
    /**
     * @brief Gibt die letzte gemessene GPU-Auslastung zurück
     * @return GPU-Auslastung in Prozent (0-100)
     */
    int getGpuUsage() const { return m_gpuUsage; }
    
    /**
     * @brief Gibt den Verlauf der CPU-Temperatur zurück
     * @return Vektor mit Temperaturwerten
     */
    QVector<int> getCpuTemperatureHistory() const { return m_cpuTemperatureHistory; }
    
    /**
     * @brief Gibt den Verlauf der CPU-Auslastung zurück
     * @return Vektor mit Auslastungswerten
     */
    QVector<int> getCpuUsageHistory() const { return m_cpuUsageHistory; }
    
    /**
     * @brief Gibt den Verlauf der GPU-Temperatur zurück
     * @return Vektor mit Temperaturwerten
     */
    QVector<int> getGpuTemperatureHistory() const { return m_gpuTemperatureHistory; }
    
    /**
     * @brief Gibt den Verlauf der GPU-Auslastung zurück
     * @return Vektor mit Auslastungswerten
     */
    QVector<int> getGpuUsageHistory() const { return m_gpuUsageHistory; }

signals:
    /**
     * @brief Signal wird ausgelöst, wenn sich die CPU-Temperatur ändert
     * @param temperature Neue CPU-Temperatur
     */
    void cpuTemperatureChanged(int temperature);
    
    /**
     * @brief Signal wird ausgelöst, wenn sich die CPU-Auslastung ändert
     * @param usage Neue CPU-Auslastung
     */
    void cpuUsageChanged(int usage);
    
    /**
     * @brief Signal wird ausgelöst, wenn sich die GPU-Temperatur ändert
     * @param temperature Neue GPU-Temperatur
     */
    void gpuTemperatureChanged(int temperature);
    
    /**
     * @brief Signal wird ausgelöst, wenn sich die GPU-Auslastung ändert
     * @param usage Neue GPU-Auslastung
     */
    void gpuUsageChanged(int usage);
    
    /**
     * @brief Signal wird ausgelöst, wenn alle Sensoren aktualisiert wurden
     */
    void sensorsUpdated();

private slots:
    /**
     * @brief Aktualisiert alle Sensorwerte
     */
    void updateSensors();

private:
    /**
     * @brief Initialisiert die WMI-Verbindung (nur Windows)
     * @return true bei erfolgreicher Initialisierung
     */
    bool initializeWMI();
    
    /**
     * @brief Bereinigt die WMI-Verbindung (nur Windows)
     */
    void cleanupWMI();
    
    /**
     * @brief Führt eine WMI-Abfrage aus (nur Windows)
     * @param wqlQuery Die WQL-Abfrage
     * @param propertyName Der Name der abzufragenden Eigenschaft
     * @return Ergebnis der Abfrage als QVariant
     */
    QVariant executeWmiQuery(const QString &wqlQuery, const QString &propertyName);
    
    /**
     * @brief Initialisiert die PDH-Abfrage für CPU-Auslastung (nur Windows)
     * @return true bei erfolgreicher Initialisierung
     */
    bool initializePdhQuery();
    
    /**
     * @brief Bereinigt die PDH-Abfrage (nur Windows)
     */
    void cleanupPdhQuery();
    
    /**
     * @brief Generiert simulierte Sensorwerte für Testzwecke
     * @param min Minimaler Wert
     * @param max Maximaler Wert
     * @param current Aktueller Wert
     * @param maxChange Maximale Änderung pro Aktualisierung
     * @return Simulierter Sensorwert
     */
    int generateSimulatedValue(int min, int max, int current, int maxChange);

private:
    QTimer *m_updateTimer;
    int m_updateInterval;
    
    // Aktuelle Sensorwerte
    int m_cpuTemperature;
    int m_cpuUsage;
    int m_gpuTemperature;
    int m_gpuUsage;
    
    // Verlauf der Sensorwerte (für Diagramme)
    QVector<int> m_cpuTemperatureHistory;
    QVector<int> m_cpuUsageHistory;
    QVector<int> m_gpuTemperatureHistory;
    QVector<int> m_gpuUsageHistory;
    
    // Maximale Anzahl von Verlaufsdatenpunkten
    static const int MAX_HISTORY_SIZE = 60;
    
#ifdef Q_OS_WIN
    // WMI-Objekte (nur Windows)
#ifndef __MINGW32__
    bool m_wmiInitialized;
    IWbemLocator *m_pWbemLocator;
    IWbemServices *m_pWbemServices;
#endif
    
    // PDH-Objekte für CPU-Auslastung (nur Windows)
    bool m_pdhInitialized;
    PDH_HQUERY m_cpuQuery;
    PDH_HCOUNTER m_cpuTotal;
#endif

    // Flags für die Verfügbarkeit von Sensoren
    bool m_hasCpuTemperature;
    bool m_hasGpuTemperature;
};
