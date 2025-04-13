#pragma once

#include "devices/irgbdevice.h"
#include <QObject>
#include <QColor>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief Repräsentiert ein ASUS RGB-Gerät
 * 
 * Diese Klasse implementiert das IRGBDevice-Interface für ASUS RGB-Geräte.
 * Sie kann entweder mit der ASUS Aura SDK oder im Simulationsmodus arbeiten.
 */
class AsusRGBDevice : public QObject, public IRGBDevice
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param id Eindeutige ID des Geräts
     * @param name Anzeigename des Geräts
     * @param type Typ des Geräts (z.B. Motherboard, GPU, etc.)
     * @param parent Parent-QObject
     */
    AsusRGBDevice(const QString &id, const QString &name, const QString &type, QObject *parent = nullptr);

    /**
     * @brief Destruktor
     */
    ~AsusRGBDevice() override;

    // IRGBDevice-Interface-Implementierung
    QString getId() const override;
    QString getDisplayName() const override;
    QString getType() const override;
    QStringList getSupportedEffects() const;
    bool setColor(const QColor &color) override;
    bool setEffect(const QString &effectName, const QVariantMap &params = QVariantMap()) override;
    QColor getColor() const override;
    QString getActiveEffect() const override;
    QVariantMap getEffectParameters() const override;
    bool isConnected() const override;

private:
    QString m_id;
    QString m_name;
    QString m_type;
    QColor m_currentColor;
    QString m_currentEffect;
    QVariantMap m_currentParameters;
    QStringList m_supportedEffects;
};
