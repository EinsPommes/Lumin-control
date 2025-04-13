#pragma once

#include <QObject>
#include <QString>
#include <QColor>
#include <QTimer>
#include <QVariantMap>

/**
 * @brief Basisklasse für RGB-Effekte
 * 
 * Diese Klasse dient als Basis für verschiedene RGB-Effekte wie statische Farben,
 * Farbverläufe, Pulsieren usw.
 */
class Effect : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Aufzählung der verfügbaren Effekttypen
     */
    enum Type {
        Static,     ///< Statische Farbe
        Breathing,  ///< Pulsierender Effekt
        Rainbow,    ///< Regenbogen-Effekt
        Wave,       ///< Welleneffekt
        Reactive    ///< Reaktiver Effekt
    };
    
    /**
     * @brief Konstruktor
     * @param type Effekttyp
     * @param parent Parent-Objekt
     */
    explicit Effect(Type type, QObject *parent = nullptr);
    
    /**
     * @brief Destruktor
     */
    virtual ~Effect();
    
    /**
     * @brief Gibt den Effekttyp zurück
     * @return Effekttyp
     */
    Type getType() const;
    
    /**
     * @brief Gibt den Namen des Effekts zurück
     * @return Name des Effekts
     */
    QString getName() const;
    
    /**
     * @brief Gibt die aktuelle Farbe des Effekts zurück
     * @return Aktuelle Farbe
     */
    virtual QColor getCurrentColor() const = 0;
    
    /**
     * @brief Setzt Parameter für den Effekt
     * @param parameters Parameter-Map
     */
    virtual void setParameters(const QVariantMap &parameters);
    
    /**
     * @brief Startet den Effekt
     */
    virtual void start();
    
    /**
     * @brief Stoppt den Effekt
     */
    virtual void stop();
    
    /**
     * @brief Prüft, ob der Effekt aktiv ist
     * @return true wenn aktiv, false wenn nicht
     */
    bool isActive() const;
    
    /**
     * @brief Erstellt einen Effekt basierend auf dem Typ
     * @param type Effekttyp
     * @param parameters Parameter für den Effekt
     * @param parent Parent-Objekt
     * @return Pointer auf den erstellten Effekt
     */
    static Effect* createEffect(Type type, const QVariantMap &parameters = QVariantMap(), QObject *parent = nullptr);
    
    /**
     * @brief Konvertiert einen Effektnamen in einen Effekttyp
     * @param name Name des Effekts
     * @return Effekttyp
     */
    static Type typeFromName(const QString &name);
    
    /**
     * @brief Konvertiert einen Effekttyp in einen Effektnamen
     * @param type Effekttyp
     * @return Name des Effekts
     */
    static QString nameFromType(Type type);

signals:
    /**
     * @brief Signal, das bei Farbänderung ausgelöst wird
     * @param color Die neue Farbe
     */
    void colorChanged(const QColor &color);

protected:
    Type m_type;
    bool m_active;
    QTimer *m_timer;
    QVariantMap m_parameters;
};

/**
 * @brief Statischer Farbeffekt
 */
class StaticEffect : public Effect
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param color Farbe
     * @param parent Parent-Objekt
     */
    explicit StaticEffect(const QColor &color = Qt::white, QObject *parent = nullptr);
    
    /**
     * @brief Gibt die aktuelle Farbe zurück
     * @return Aktuelle Farbe
     */
    QColor getCurrentColor() const override;
    
    /**
     * @brief Setzt Parameter für den Effekt
     * @param parameters Parameter-Map
     */
    void setParameters(const QVariantMap &parameters) override;

private:
    QColor m_color;
};

/**
 * @brief Pulsierender Farbeffekt
 */
class BreathingEffect : public Effect
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param color Farbe
     * @param speed Geschwindigkeit (ms)
     * @param parent Parent-Objekt
     */
    explicit BreathingEffect(const QColor &color = Qt::white, int speed = 2000, QObject *parent = nullptr);
    
    /**
     * @brief Gibt die aktuelle Farbe zurück
     * @return Aktuelle Farbe
     */
    QColor getCurrentColor() const override;
    
    /**
     * @brief Setzt Parameter für den Effekt
     * @param parameters Parameter-Map
     */
    void setParameters(const QVariantMap &parameters) override;
    
    /**
     * @brief Startet den Effekt
     */
    void start() override;

private slots:
    /**
     * @brief Aktualisiert den Effekt
     */
    void update();

private:
    QColor m_color;
    int m_speed;
    qreal m_intensity;
    bool m_increasing;
};

/**
 * @brief Regenbogen-Farbeffekt
 */
class RainbowEffect : public Effect
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param speed Geschwindigkeit (ms)
     * @param parent Parent-Objekt
     */
    explicit RainbowEffect(int speed = 2000, QObject *parent = nullptr);
    
    /**
     * @brief Gibt die aktuelle Farbe zurück
     * @return Aktuelle Farbe
     */
    QColor getCurrentColor() const override;
    
    /**
     * @brief Setzt Parameter für den Effekt
     * @param parameters Parameter-Map
     */
    void setParameters(const QVariantMap &parameters) override;
    
    /**
     * @brief Startet den Effekt
     */
    void start() override;

private slots:
    /**
     * @brief Aktualisiert den Effekt
     */
    void update();

private:
    int m_speed;
    int m_hue;
};

/**
 * @brief Welleneffekt
 */
class WaveEffect : public Effect
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param color1 Erste Farbe
     * @param color2 Zweite Farbe
     * @param speed Geschwindigkeit (ms)
     * @param parent Parent-Objekt
     */
    explicit WaveEffect(const QColor &color1 = Qt::blue, const QColor &color2 = Qt::cyan, int speed = 2000, QObject *parent = nullptr);
    
    /**
     * @brief Gibt die aktuelle Farbe zurück
     * @return Aktuelle Farbe
     */
    QColor getCurrentColor() const override;
    
    /**
     * @brief Setzt Parameter für den Effekt
     * @param parameters Parameter-Map
     */
    void setParameters(const QVariantMap &parameters) override;
    
    /**
     * @brief Startet den Effekt
     */
    void start() override;

private slots:
    /**
     * @brief Aktualisiert den Effekt
     */
    void update();

private:
    QColor m_color1;
    QColor m_color2;
    int m_speed;
    qreal m_position;
    bool m_forward;
};

/**
 * @brief Reaktiver Effekt
 */
class ReactiveEffect : public Effect
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param color Farbe
     * @param duration Dauer (ms)
     * @param parent Parent-Objekt
     */
    explicit ReactiveEffect(const QColor &color = Qt::white, int duration = 500, QObject *parent = nullptr);
    
    /**
     * @brief Gibt die aktuelle Farbe zurück
     * @return Aktuelle Farbe
     */
    QColor getCurrentColor() const override;
    
    /**
     * @brief Setzt Parameter für den Effekt
     * @param parameters Parameter-Map
     */
    void setParameters(const QVariantMap &parameters) override;
    
    /**
     * @brief Startet den Effekt
     */
    void start() override;
    
    /**
     * @brief Löst den reaktiven Effekt aus
     */
    void trigger();

private slots:
    /**
     * @brief Aktualisiert den Effekt
     */
    void update();

private:
    QColor m_color;
    QColor m_baseColor;
    int m_duration;
    qreal m_intensity;
    QTimer m_fadeTimer;
};
