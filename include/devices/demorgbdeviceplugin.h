#pragma once

#include "irgbdeviceplugin.h"
#include "irgbdevice.h"
#include <QObject>
#include <QList>
#include <QColor>

/**
 * @brief Demo-Implementierung eines RGB-Geräts
 */
class DemoRGBDevice : public QObject, public IRGBDevice {
    Q_OBJECT

public:
    explicit DemoRGBDevice(const QString &id, const QString &displayName, const QString &type, QObject *parent = nullptr);
    
    QString getId() const override;
    QString getDisplayName() const override;
    QString getType() const override;
    bool setColor(const QColor &color) override;
    bool setEffect(const QString &effectName, const QVariantMap &parameters = QVariantMap()) override;
    bool isConnected() const override;

private:
    QString m_id;
    QString m_displayName;
    QString m_type;
    QColor m_currentColor;
    QString m_currentEffect;
    bool m_connected;
};

/**
 * @brief Demo-Implementierung eines RGB-Geräteplugins
 */
class DemoRGBDevicePlugin : public QObject, public IRGBDevicePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.lumincontrol.IRGBDevicePlugin" FILE "demorgbdeviceplugin.json")
    Q_INTERFACES(IRGBDevicePlugin)

public:
    explicit DemoRGBDevicePlugin(QObject *parent = nullptr);
    ~DemoRGBDevicePlugin();
    
    // IRGBDevicePlugin-Interface-Implementierung
    bool initialize() override;
    QString getName() const override;
    QString getVersion() const override;
    QString getAuthor() const override;
    QList<IRGBDevice*> getDevices() const override;

private:
    QList<DemoRGBDevice*> m_devices;
};
