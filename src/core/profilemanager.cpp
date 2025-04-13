#include "core/profilemanager.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDebug>
#include <nlohmann/json.hpp>

// Verwende nlohmann::json für die JSON-Operationen
using json = nlohmann::json;

ProfileManager::ProfileManager(RGBController *rgbController, QObject *parent)
    : QObject(parent)
    , m_rgbController(rgbController)
    , m_profilesDir(QDir::current())
    , m_defaultProfilePath(QDir::current().filePath("config/default_profile.txt"))
{
    // Profilordner festlegen
    m_profilesDir.cd("profiles");
    
    // Profilordner erstellen, falls er nicht existiert
    ensureProfileDirectoryExists();
    
    // Konfigurationsordner erstellen, falls er nicht existiert
    QDir configDir = QDir::current();
    if (!configDir.exists("config")) {
        configDir.mkdir("config");
    }
}

ProfileManager::~ProfileManager()
{
    // Nichts zu tun, QObject kümmert sich um die Ressourcenfreigabe
}

QStringList ProfileManager::getAvailableProfiles() const
{
    QStringList profiles;
    
    if (m_profilesDir.exists()) {
        QStringList filters;
        filters << "*.json";
        
        // Nicht-konstante Kopie erstellen, um setNameFilters aufzurufen
        QDir profilesDir = m_profilesDir;
        profilesDir.setNameFilters(filters);
        
        QStringList fileNames = profilesDir.entryList(filters, QDir::Files);
        for (const QString &fileName : fileNames) {
            // Dateiendung entfernen
            QString profileName = fileName;
            profileName.chop(5); // ".json" entfernen
            profiles << profileName;
        }
    }
    
    return profiles;
}

bool ProfileManager::loadProfile(const QString &profileName)
{
    QString profilePath = getProfilePath(profileName);
    QFile file(profilePath);
    
    if (!file.exists()) {
        emit error(QString("Profil '%1' existiert nicht").arg(profileName));
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Fehler beim Öffnen der Profildatei '%1'").arg(profileName));
        return false;
    }
    
    try {
        // Datei mit nlohmann::json einlesen
        json profileJson = json::parse(file.readAll().toStdString());
        file.close();
        
        // JSON in QJsonObject konvertieren
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(profileJson.dump()));
        QJsonObject jsonObj = doc.object();
        
        // JSON auf Geräte anwenden
        bool success = applyJsonToDevices(jsonObj);
        
        if (success) {
            emit profileLoaded(profileName);
        } else {
            emit error(QString("Fehler beim Anwenden des Profils '%1'").arg(profileName));
        }
        
        return success;
    } catch (const std::exception &e) {
        file.close();
        emit error(QString("Fehler beim Parsen der Profildatei '%1': %2").arg(profileName).arg(e.what()));
        return false;
    }
}

bool ProfileManager::saveProfile(const QString &profileName, bool includeTemperatureRules)
{
    if (!ensureProfileDirectoryExists()) {
        emit error("Fehler beim Erstellen des Profilordners");
        return false;
    }
    
    QString profilePath = getProfilePath(profileName);
    QFile file(profilePath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(QString("Fehler beim Erstellen der Profildatei '%1'").arg(profileName));
        return false;
    }
    
    try {
        // Gerätezustände in JSON konvertieren
        QJsonObject jsonObj = deviceStatesToJson(includeTemperatureRules);
        QJsonDocument doc(jsonObj);
        
        // QJsonDocument in nlohmann::json konvertieren
        json profileJson = json::parse(doc.toJson().toStdString());
        
        // In Datei schreiben (mit Einrückung für bessere Lesbarkeit)
        file.write(QByteArray::fromStdString(profileJson.dump(4)));
        file.close();
        
        emit profileSaved(profileName);
        return true;
    } catch (const std::exception &e) {
        file.close();
        emit error(QString("Fehler beim Speichern des Profils '%1': %2").arg(profileName).arg(e.what()));
        return false;
    }
}

bool ProfileManager::deleteProfile(const QString &profileName)
{
    QString profilePath = getProfilePath(profileName);
    QFile file(profilePath);
    
    if (!file.exists()) {
        emit error(QString("Profil '%1' existiert nicht").arg(profileName));
        return false;
    }
    
    if (file.remove()) {
        // Wenn das gelöschte Profil das Standardprofil war, Standardprofil zurücksetzen
        if (getDefaultProfile() == profileName) {
            QFile defaultFile(m_defaultProfilePath);
            defaultFile.remove();
        }
        
        emit profileDeleted(profileName);
        return true;
    } else {
        emit error(QString("Fehler beim Löschen des Profils '%1'").arg(profileName));
        return false;
    }
}

bool ProfileManager::setDefaultProfile(const QString &profileName)
{
    if (!profileExists(profileName)) {
        emit error(QString("Profil '%1' existiert nicht").arg(profileName));
        return false;
    }
    
    QFile file(m_defaultProfilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit error("Fehler beim Speichern des Standardprofils");
        return false;
    }
    
    file.write(profileName.toUtf8());
    file.close();
    
    return true;
}

QString ProfileManager::getDefaultProfile() const
{
    QFile file(m_defaultProfilePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    
    QString profileName = QString::fromUtf8(file.readAll()).trimmed();
    file.close();
    
    return profileName;
}

bool ProfileManager::loadDefaultProfile()
{
    QString defaultProfile = getDefaultProfile();
    if (defaultProfile.isEmpty()) {
        return false;
    }
    
    return loadProfile(defaultProfile);
}

bool ProfileManager::profileExists(const QString &profileName) const
{
    QString profilePath = getProfilePath(profileName);
    return QFile::exists(profilePath);
}

bool ProfileManager::ensureProfileDirectoryExists()
{
    if (!m_profilesDir.exists()) {
        QDir baseDir = QDir::current();
        if (!baseDir.mkdir("profiles")) {
            return false;
        }
    }
    
    return true;
}

QString ProfileManager::getProfilePath(const QString &profileName) const
{
    return m_profilesDir.filePath(profileName + ".json");
}

QJsonObject ProfileManager::deviceStatesToJson(bool includeTemperatureRules) const
{
    QJsonObject root;
    QJsonArray devicesArray;
    
    // Geräte-Informationen speichern
    for (IRGBDevice *device : m_rgbController->getDevices()) {
        QJsonObject deviceObj;
        deviceObj["id"] = device->getId();
        deviceObj["name"] = device->getDisplayName();
        
        // Aktuelle Farbe speichern
        QColor color = device->getColor();
        QJsonObject colorObj;
        colorObj["r"] = color.red();
        colorObj["g"] = color.green();
        colorObj["b"] = color.blue();
        deviceObj["color"] = colorObj;
        
        // Aktiven Effekt speichern
        QString effectName = device->getActiveEffect();
        deviceObj["effect"] = effectName;
        
        // Effekt-Parameter speichern (falls vorhanden)
        if (!effectName.isEmpty() && effectName != "Statisch") {
            QVariantMap effectParams = device->getEffectParameters();
            QJsonObject paramsObj;
            
            for (auto it = effectParams.begin(); it != effectParams.end(); ++it) {
                paramsObj[it.key()] = QJsonValue::fromVariant(it.value());
            }
            
            deviceObj["effectParameters"] = paramsObj;
        }
        
        devicesArray.append(deviceObj);
    }
    
    root["devices"] = devicesArray;
    
    // Temperaturregeln speichern, falls gewünscht
    if (includeTemperatureRules) {
        QJsonObject tempRulesObj;
        tempRulesObj["enabled"] = m_rgbController->isTemperatureLinkingEnabled();
        root["temperatureRules"] = tempRulesObj;
    }
    
    return root;
}

bool ProfileManager::applyJsonToDevices(const QJsonObject &json)
{
    if (!json.contains("devices")) {
        return false;
    }
    
    QJsonArray devicesArray = json["devices"].toArray();
    
    // Geräte-Informationen anwenden
    for (const QJsonValue &deviceValue : devicesArray) {
        QJsonObject deviceObj = deviceValue.toObject();
        
        QString deviceId = deviceObj["id"].toString();
        IRGBDevice *device = m_rgbController->getDeviceById(deviceId);
        
        if (!device) {
            qDebug() << "Gerät mit ID" << deviceId << "nicht gefunden, überspringe...";
            continue;
        }
        
        // Effekt anwenden, falls vorhanden
        if (deviceObj.contains("effect")) {
            QString effectName = deviceObj["effect"].toString();
            
            if (!effectName.isEmpty() && effectName != "Statisch") {
                QVariantMap effectParams;
                
                // Effekt-Parameter laden, falls vorhanden
                if (deviceObj.contains("effectParameters")) {
                    QJsonObject paramsObj = deviceObj["effectParameters"].toObject();
                    
                    for (auto it = paramsObj.begin(); it != paramsObj.end(); ++it) {
                        effectParams[it.key()] = it.value().toVariant();
                    }
                }
                
                m_rgbController->setEffectForDevice(device, effectName, effectParams);
            } else {
                // Statische Farbe anwenden
                if (deviceObj.contains("color")) {
                    QJsonObject colorObj = deviceObj["color"].toObject();
                    QColor color(
                        colorObj["r"].toInt(),
                        colorObj["g"].toInt(),
                        colorObj["b"].toInt()
                    );
                    
                    m_rgbController->setColorForDevice(device, color);
                }
            }
        }
    }
    
    // Temperaturregeln anwenden, falls vorhanden
    if (json.contains("temperatureRules")) {
        QJsonObject tempRulesObj = json["temperatureRules"].toObject();
        bool enabled = tempRulesObj["enabled"].toBool();
        m_rgbController->setTemperatureLinking(enabled);
    }
    
    return true;
}
