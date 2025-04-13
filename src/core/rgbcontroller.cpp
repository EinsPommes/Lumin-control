#include "core/rgbcontroller.h"
#include <QDebug>

RGBController::RGBController(QObject *parent)
    : QObject(parent)
    , m_effectMapper(new QSignalMapper(this))
    , m_temperatureLinkingEnabled(false)
    , m_cpuTemperature(0)
    , m_gpuTemperature(0)
{
    connect(m_effectMapper, &QSignalMapper::mappedString, this, &RGBController::updateDeviceColor);
}

RGBController::~RGBController()
{
    // Alle Effekte löschen
    qDeleteAll(m_deviceEffects);
    m_deviceEffects.clear();
}

void RGBController::registerDevice(IRGBDevice *device)
{
    if (!device) return;
    
    if (!m_devices.contains(device)) {
        m_devices.append(device);
        qDebug() << "Gerät registriert:" << device->getDisplayName();
    }
}

void RGBController::unregisterDevice(IRGBDevice *device)
{
    if (!device) return;
    
    if (m_devices.contains(device)) {
        m_devices.removeAll(device);
        
        // Effekt für das Gerät entfernen
        QString deviceId = device->getId();
        if (m_deviceEffects.contains(deviceId)) {
            delete m_deviceEffects[deviceId];
            m_deviceEffects.remove(deviceId);
        }
        
        qDebug() << "Gerät entfernt:" << device->getDisplayName();
    }
}

QList<IRGBDevice*> RGBController::getDevices() const
{
    return m_devices;
}

IRGBDevice* RGBController::getDeviceById(const QString &id) const
{
    for (IRGBDevice *device : m_devices) {
        if (device->getId() == id) {
            return device;
        }
    }
    
    return nullptr;
}

bool RGBController::setColorForDevice(IRGBDevice *device, const QColor &color)
{
    if (!device || !device->isConnected()) {
        return false;
    }
    
    bool success = device->setColor(color);
    
    if (success) {
        QString deviceId = device->getId();
        
        // Wenn ein Effekt aktiv ist, diesen stoppen
        if (m_deviceEffects.contains(deviceId)) {
            m_deviceEffects[deviceId]->stop();
            delete m_deviceEffects[deviceId];
            m_deviceEffects.remove(deviceId);
        }
        
        // Statischen Effekt erstellen
        StaticEffect *staticEffect = new StaticEffect(color, this);
        m_deviceEffects[deviceId] = staticEffect;
        
        emit colorChanged(deviceId, color);
        emit actionSuccess(QString("Farbe %1 für Gerät '%2' gesetzt").arg(color.name()).arg(device->getDisplayName()));
    } else {
        emit actionError(QString("Fehler beim Setzen der Farbe für Gerät '%1'").arg(device->getDisplayName()));
    }
    
    return success;
}

int RGBController::setColorForDevices(const QList<IRGBDevice*> &devices, const QColor &color)
{
    int successCount = 0;
    
    for (IRGBDevice *device : devices) {
        if (setColorForDevice(device, color)) {
            successCount++;
        }
    }
    
    if (successCount > 0) {
        emit actionSuccess(QString("Farbe %1 für %2 Gerät(e) gesetzt").arg(color.name()).arg(successCount));
    } else if (!devices.isEmpty()) {
        emit actionError("Fehler beim Setzen der Farbe für alle Geräte");
    }
    
    return successCount;
}

int RGBController::setColorForAllDevices(const QColor &color)
{
    return setColorForDevices(m_devices, color);
}

bool RGBController::setEffectForDevice(IRGBDevice *device, const QString &effectName, const QVariantMap &parameters)
{
    if (!device || !device->isConnected()) {
        return false;
    }
    
    QString deviceId = device->getId();
    
    // Alten Effekt entfernen, falls vorhanden
    if (m_deviceEffects.contains(deviceId)) {
        m_deviceEffects[deviceId]->stop();
        delete m_deviceEffects[deviceId];
        m_deviceEffects.remove(deviceId);
    }
    
    // Neuen Effekt erstellen
    Effect *effect = createEffect(effectName, parameters);
    if (!effect) {
        emit actionError(QString("Unbekannter Effekt: %1").arg(effectName));
        return false;
    }
    
    // Effekt mit Gerät verbinden
    connect(effect, &Effect::colorChanged, [this, device](const QColor &color) {
        device->setColor(color);
        emit colorChanged(device->getId(), color);
    });
    
    // Effekt starten
    effect->start();
    
    // Effekt speichern
    m_deviceEffects[deviceId] = effect;
    
    // Effekt-Mapper verbinden
    connect(effect, &Effect::colorChanged, m_effectMapper, [this, deviceId]() {
        m_effectMapper->setMapping(qobject_cast<QObject*>(sender()), deviceId);
    });
    
    bool success = device->setEffect(effectName, parameters);
    
    if (success) {
        emit effectChanged(deviceId, effectName);
        emit actionSuccess(QString("Effekt '%1' für Gerät '%2' gesetzt").arg(effectName).arg(device->getDisplayName()));
    } else {
        emit actionError(QString("Fehler beim Setzen des Effekts für Gerät '%1'").arg(device->getDisplayName()));
    }
    
    return success;
}

int RGBController::setEffectForDevices(const QList<IRGBDevice*> &devices, const QString &effectName, const QVariantMap &parameters)
{
    int successCount = 0;
    
    for (IRGBDevice *device : devices) {
        if (setEffectForDevice(device, effectName, parameters)) {
            successCount++;
        }
    }
    
    if (successCount > 0) {
        emit actionSuccess(QString("Effekt '%1' für %2 Gerät(e) gesetzt").arg(effectName).arg(successCount));
    } else if (!devices.isEmpty()) {
        emit actionError(QString("Fehler beim Setzen des Effekts '%1' für alle Geräte").arg(effectName));
    }
    
    return successCount;
}

int RGBController::setEffectForAllDevices(const QString &effectName, const QVariantMap &parameters)
{
    return setEffectForDevices(m_devices, effectName, parameters);
}

Effect* RGBController::createEffect(const QString &effectName, const QVariantMap &parameters)
{
    Effect::Type type = Effect::typeFromName(effectName);
    return Effect::createEffect(type, parameters, this);
}

QStringList RGBController::getAvailableEffects() const
{
    QStringList effects;
    effects << "Statisch" << "Atmen" << "Regenbogen" << "Welle" << "Reaktiv";
    return effects;
}

void RGBController::setTemperatureLinking(bool enabled)
{
    m_temperatureLinkingEnabled = enabled;
    
    if (enabled) {
        // Temperaturbasierte Farben anwenden
        updateTemperatures(m_cpuTemperature, m_gpuTemperature);
    }
}

bool RGBController::isTemperatureLinkingEnabled() const
{
    return m_temperatureLinkingEnabled;
}

void RGBController::updateTemperatures(int cpuTemp, int gpuTemp)
{
    m_cpuTemperature = cpuTemp;
    m_gpuTemperature = gpuTemp;
    
    if (m_temperatureLinkingEnabled) {
        // Höchste Temperatur für die Farbgebung verwenden
        int maxTemp = qMax(cpuTemp, gpuTemp);
        QColor tempColor = Color::fromTemperature(maxTemp);
        
        // Farbe auf alle Geräte anwenden
        setColorForAllDevices(tempColor);
    }
}

void RGBController::updateDeviceColor(const QString &deviceId)
{
    if (!m_deviceEffects.contains(deviceId)) {
        return;
    }
    
    Effect *effect = m_deviceEffects[deviceId];
    IRGBDevice *device = getDeviceById(deviceId);
    
    if (device && effect) {
        QColor color = effect->getCurrentColor();
        device->setColor(color);
        emit colorChanged(deviceId, color);
    }
}
