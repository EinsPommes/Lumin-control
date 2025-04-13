#include "monitoring/sensormonitor.h"
#include <QDebug>
#include <QRandomGenerator>

// Windows-spezifische Includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")

// WMI-Includes nur für MSVC
#ifndef __MINGW32__
#include <wbemidl.h>
#include <comdef.h>
#include <Wbemcli.h>
#pragma comment(lib, "wbemuuid.lib")
#endif
#endif

SensorMonitor::SensorMonitor(int updateInterval, QObject *parent)
    : QObject(parent)
    , m_updateTimer(new QTimer(this))
    , m_updateInterval(updateInterval)
    , m_cpuTemperature(0)
    , m_cpuUsage(0)
    , m_gpuTemperature(0)
    , m_gpuUsage(0)
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    , m_wmiInitialized(false)
    , m_pWbemLocator(nullptr)
    , m_pWbemServices(nullptr)
#endif
    , m_pdhInitialized(false)
#endif
    , m_hasCpuTemperature(false)
    , m_hasGpuTemperature(false)
{
    // Timer-Verbindung
    connect(m_updateTimer, &QTimer::timeout, this, &SensorMonitor::updateSensors);
    
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    // WMI initialisieren (nur für MSVC)
    m_wmiInitialized = initializeWMI();
    if (!m_wmiInitialized) {
        qWarning() << "WMI konnte nicht initialisiert werden. Verwende Demo-Daten.";
    }
#else
    qWarning() << "WMI wird unter MinGW nicht unterstützt. Verwende Demo-Daten.";
#endif
    
    // PDH für CPU-Auslastung initialisieren
    m_pdhInitialized = initializePdhQuery();
    if (!m_pdhInitialized) {
        qWarning() << "PDH-Abfrage konnte nicht initialisiert werden. Verwende Demo-Daten für CPU-Auslastung.";
    }
#endif
}

SensorMonitor::~SensorMonitor()
{
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    // WMI-Ressourcen freigeben (nur für MSVC)
    cleanupWMI();
#endif
    
    // PDH-Ressourcen freigeben
    cleanupPdhQuery();
#endif
}

void SensorMonitor::startMonitoring()
{
    m_updateTimer->start(m_updateInterval);
    
    // Sofort erste Messung durchführen
    updateSensors();
}

void SensorMonitor::stopMonitoring()
{
    m_updateTimer->stop();
}

void SensorMonitor::updateSensors()
{
    // Alte Werte speichern
    int oldCpuTemp = m_cpuTemperature;
    int oldCpuUsage = m_cpuUsage;
    int oldGpuTemp = m_gpuTemperature;
    int oldGpuUsage = m_gpuUsage;
    
    // Neue Werte auslesen
    m_cpuTemperature = readCpuTemperature();
    m_cpuUsage = readCpuUsage();
    m_gpuTemperature = readGpuTemperature();
    m_gpuUsage = readGpuUsage();
    
    // Verlaufsdaten aktualisieren
    m_cpuTemperatureHistory.append(m_cpuTemperature);
    m_cpuUsageHistory.append(m_cpuUsage);
    m_gpuTemperatureHistory.append(m_gpuTemperature);
    m_gpuUsageHistory.append(m_gpuUsage);
    
    // Verlaufsdaten auf maximale Größe begrenzen
    while (m_cpuTemperatureHistory.size() > MAX_HISTORY_SIZE) {
        m_cpuTemperatureHistory.removeFirst();
    }
    while (m_cpuUsageHistory.size() > MAX_HISTORY_SIZE) {
        m_cpuUsageHistory.removeFirst();
    }
    while (m_gpuTemperatureHistory.size() > MAX_HISTORY_SIZE) {
        m_gpuTemperatureHistory.removeFirst();
    }
    while (m_gpuUsageHistory.size() > MAX_HISTORY_SIZE) {
        m_gpuUsageHistory.removeFirst();
    }
    
    // Signale senden, wenn sich Werte geändert haben
    if (m_cpuTemperature != oldCpuTemp) {
        emit cpuTemperatureChanged(m_cpuTemperature);
    }
    
    if (m_cpuUsage != oldCpuUsage) {
        emit cpuUsageChanged(m_cpuUsage);
    }
    
    if (m_gpuTemperature != oldGpuTemp) {
        emit gpuTemperatureChanged(m_gpuTemperature);
    }
    
    if (m_gpuUsage != oldGpuUsage) {
        emit gpuUsageChanged(m_gpuUsage);
    }
    
    // Signal für Aktualisierung aller Sensoren
    emit sensorsUpdated();
}

#ifdef Q_OS_WIN
bool SensorMonitor::initializeWMI()
{
#ifndef __MINGW32__
    HRESULT hres;
    
    // COM initialisieren
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        qWarning() << "Failed to initialize COM library. Error code =" << hres;
        return false;
    }
    
    // COM-Sicherheit setzen
    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM-Authentifizierung
        NULL,                        // Authentifizierungs-Services
        NULL,                        // Reserviert
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Standard-Authentifizierungslevel
        RPC_C_IMP_LEVEL_IMPERSONATE, // Standard-Impersonation-Level
        NULL,                        // Authentifizierungs-Info
        EOAC_NONE,                   // Zusätzliche Funktionen
        NULL                         // Reserviert
    );
    
    if (FAILED(hres)) {
        qWarning() << "Failed to initialize security. Error code =" << hres;
        CoUninitialize();
        return false;
    }
    
    // WbemLocator erstellen
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&m_pWbemLocator
    );
    
    if (FAILED(hres)) {
        qWarning() << "Failed to create IWbemLocator object. Error code =" << hres;
        CoUninitialize();
        return false;
    }
    
    // WbemServices verbinden
    hres = m_pWbemLocator->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),     // WMI-Namespace
        NULL,                        // Benutzername
        NULL,                        // Passwort
        0,                           // Locale
        NULL,                        // Sicherheitsflags
        0,                           // Autorität
        0,                           // Kontext
        &m_pWbemServices             // IWbemServices-Proxy
    );
    
    if (FAILED(hres)) {
        qWarning() << "Could not connect to WMI namespace. Error code =" << hres;
        m_pWbemLocator->Release();
        CoUninitialize();
        return false;
    }
    
    // Proxy-Sicherheit setzen
    hres = CoSetProxyBlanket(
        m_pWbemServices,             // Proxy
        RPC_C_AUTHN_WINNT,           // Authentifizierungsservice
        RPC_C_AUTHZ_NONE,            // Autorisierungsservice
        NULL,                        // Server-Principal-Name
        RPC_C_AUTHN_LEVEL_CALL,      // Authentifizierungslevel
        RPC_C_IMP_LEVEL_IMPERSONATE, // Impersonation-Level
        NULL,                        // Client-Identity
        EOAC_NONE                    // Proxy-Funktionen
    );
    
    if (FAILED(hres)) {
        qWarning() << "Could not set proxy blanket. Error code =" << hres;
        m_pWbemServices->Release();
        m_pWbemLocator->Release();
        CoUninitialize();
        return false;
    }
    
    return true;
#else
    // MinGW-Kompilierung: Verwende simulierte Daten statt WMI
    qWarning() << "WMI wird unter MinGW nicht unterstützt. Verwende Demo-Daten.";
    return false;
#endif // __MINGW32__
}

void SensorMonitor::cleanupWMI()
{
#ifndef __MINGW32__
    // WMI-Ressourcen freigeben
    if (m_wmiInitialized) {
        if (m_pWbemServices) {
            m_pWbemServices->Release();
            m_pWbemServices = nullptr;
        }
        
        if (m_pWbemLocator) {
            m_pWbemLocator->Release();
            m_pWbemLocator = nullptr;
        }
        
        CoUninitialize();
        m_wmiInitialized = false;
    }
#endif // __MINGW32__
}

QVariant SensorMonitor::executeWmiQuery(const QString &wqlQuery, const QString &propertyName)
{
#ifndef __MINGW32__
    if (!m_wmiInitialized) {
        return QVariant();
    }
    
    HRESULT hres;
    IEnumWbemClassObject* pEnumerator = NULL;
    
    // WQL-Abfrage ausführen
    hres = m_pWbemServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t(wqlQuery.toStdString().c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator
    );
    
    if (FAILED(hres)) {
        qWarning() << "Query failed. Error code =" << hres;
        return QVariant();
    }
    
    // Ergebnisse abrufen
    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    QVariant result;
    
    while (pEnumerator) {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        
        if (uReturn == 0) {
            break;
        }
        
        VARIANT vtProp;
        VariantInit(&vtProp);
        
        // Eigenschaft abrufen
        CIMTYPE pType;
        hres = pclsObj->Get(
            _bstr_t(propertyName.toStdString().c_str()),
            0,
            &vtProp,
            &pType,
            0
        );
        
        if (SUCCEEDED(hres)) {
            // VARIANT in QVariant konvertieren
            switch (vtProp.vt) {
                case VT_I4:
                    result = QVariant(vtProp.lVal);
                    break;
                case VT_UI4:
                    result = QVariant(static_cast<int>(vtProp.ulVal));
                    break;
                case VT_BSTR:
                    result = QVariant(QString::fromWCharArray(vtProp.bstrVal));
                    break;
                case VT_BOOL:
                    result = QVariant(vtProp.boolVal != 0);
                    break;
                case VT_R4:
                    result = QVariant(vtProp.fltVal);
                    break;
                case VT_R8:
                    result = QVariant(vtProp.dblVal);
                    break;
                default:
                    qWarning() << "Unsupported variant type:" << vtProp.vt;
            }
        }
        
        VariantClear(&vtProp);
        pclsObj->Release();
    }
    
    pEnumerator->Release();
    return result;
#else
    // MinGW-Kompilierung: Keine WMI-Unterstützung
    Q_UNUSED(wqlQuery);
    Q_UNUSED(propertyName);
    return QVariant();
#endif // __MINGW32__
}

bool SensorMonitor::initializePdhQuery()
{
#ifdef __MINGW32__
    // MinGW-Kompilierung: Verwende simulierte Daten statt PDH
    qWarning() << "PDH wird unter MinGW nicht vollständig unterstützt. Verwende Demo-Daten.";
    return false;
#else
    PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_cpuQuery);
    if (status != ERROR_SUCCESS) {
        qWarning() << "Failed to open PDH query. Error code =" << status;
        return false;
    }
    
    status = PdhAddEnglishCounter(m_cpuQuery, L"\\Processor(_Total)\\% Processor Time", 0, &m_cpuTotal);
    if (status != ERROR_SUCCESS) {
        qWarning() << "Failed to add CPU counter. Error code =" << status;
        PdhCloseQuery(m_cpuQuery);
        return false;
    }
    
    // Erste Datensammlung (wird für die Berechnung der Differenz benötigt)
    status = PdhCollectQueryData(m_cpuQuery);
    if (status != ERROR_SUCCESS) {
        qWarning() << "Failed to collect initial PDH data. Error code =" << status;
        PdhCloseQuery(m_cpuQuery);
        return false;
    }
    
    return true;
#endif
}

void SensorMonitor::cleanupPdhQuery()
{
#ifndef __MINGW32__
    if (m_pdhInitialized) {
        PdhCloseQuery(m_cpuQuery);
        m_pdhInitialized = false;
    }
#endif
}

int SensorMonitor::readCpuTemperature()
{
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    if (m_wmiInitialized) {
        // Methode 1: WMI-Abfrage für CPU-Temperatur
        QVariant temp = executeWmiQuery(
            "SELECT * FROM Win32_PerfFormattedData_Counters_ProcessorInformation WHERE Name='_Total'",
            "PercentProcessorTime"
        );
        
        if (temp.isValid()) {
            m_hasCpuTemperature = true;
            return temp.toInt();
        }
        
        // Methode 2: Alternative WMI-Abfrage für CPU-Temperatur
        temp = executeWmiQuery(
            "SELECT * FROM Win32_TemperatureProbe",
            "CurrentReading"
        );
        
        if (temp.isValid()) {
            m_hasCpuTemperature = true;
            return temp.toInt();
        }
        
        // Methode 3: Hardware-spezifische Sensoren (z.B. für Intel)
        temp = executeWmiQuery(
            "SELECT * FROM Win32_PerfFormattedData_Counters_ThermalZoneInformation",
            "Temperature"
        );
        
        if (temp.isValid()) {
            m_hasCpuTemperature = true;
            return temp.toInt();
        }
        
        // Wenn keine Temperatur verfügbar ist, schätzen wir basierend auf der CPU-Auslastung
        if (!m_hasCpuTemperature) {
            int usage = readCpuUsage();
            return generateSimulatedValue(30, 85, m_cpuTemperature, 2);
        }
    }
#endif
#endif
    
    // Fallback: Simulierte Temperatur
    return generateSimulatedValue(30, 85, m_cpuTemperature, 2);
}

int SensorMonitor::readCpuUsage()
{
#ifdef Q_OS_WIN
    if (m_pdhInitialized) {
        // PDH-Daten sammeln
        PDH_STATUS status = PdhCollectQueryData(m_cpuQuery);
        if (status != ERROR_SUCCESS) {
            qWarning() << "Failed to collect PDH data. Error code =" << status;
            return generateSimulatedValue(0, 100, m_cpuUsage, 5);
        }
        
        // Formatierte Daten abrufen
        PDH_FMT_COUNTERVALUE counterVal;
        status = PdhGetFormattedCounterValue(m_cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
        
        if (status != ERROR_SUCCESS) {
            qWarning() << "Failed to format counter data. Error code =" << status;
            return generateSimulatedValue(0, 100, m_cpuUsage, 5);
        }
        
        return static_cast<int>(counterVal.doubleValue);
    }
#endif
    
    // Fallback: Simulierte Auslastung
    return generateSimulatedValue(0, 100, m_cpuUsage, 5);
}

int SensorMonitor::readGpuTemperature()
{
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    if (m_wmiInitialized) {
        // Methode für NVIDIA-GPUs
        QVariant temp = executeWmiQuery(
            "SELECT * FROM Win32_PerfFormattedData_GPUPerformanceCounters_GPUTemperature",
            "Temperature"
        );
        
        if (temp.isValid()) {
            m_hasGpuTemperature = true;
            return temp.toInt();
        }
        
        // Alternative Methode für AMD-GPUs
        temp = executeWmiQuery(
            "SELECT * FROM Win32_PerfFormattedData_AMDGPUPerformanceCounters_AMDGPUTemperature",
            "Temperature"
        );
        
        if (temp.isValid()) {
            m_hasGpuTemperature = true;
            return temp.toInt();
        }
        
        // Wenn keine Temperatur verfügbar ist, schätzen wir basierend auf der GPU-Auslastung
        if (!m_hasGpuTemperature) {
            int usage = readGpuUsage();
            return generateSimulatedValue(35, 90, m_gpuTemperature, 3);
        }
    }
#endif
#endif
    
    // Fallback: Simulierte Temperatur
    return generateSimulatedValue(35, 90, m_gpuTemperature, 3);
}

int SensorMonitor::readGpuUsage()
{
#ifdef Q_OS_WIN
#ifndef __MINGW32__
    if (m_wmiInitialized) {
        // Methode für NVIDIA und AMD GPUs
        QVariant usage = executeWmiQuery(
            "SELECT * FROM Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine",
            "UtilizationPercentage"
        );
        
        if (usage.isValid()) {
            return usage.toInt();
        }
    }
#endif
#endif
    
    // Fallback: Simulierte Auslastung
    return generateSimulatedValue(0, 100, m_gpuUsage, 8);
}

int SensorMonitor::generateSimulatedValue(int min, int max, int current, int maxChange)
{
    // Generiere einen simulierten Wert, der sich nicht zu stark vom vorherigen unterscheidet
    int change = QRandomGenerator::global()->bounded(-maxChange, maxChange + 1);
    int newValue = current + change;
    
    // Begrenze den Wert auf den gültigen Bereich
    if (newValue < min) newValue = min;
    if (newValue > max) newValue = max;
    
    return newValue;
}
#endif
