#pragma once

#include <QColor>
#include <QObject>
#include <QString>
#include <QMap>

/**
 * @brief Klasse für erweiterte Farbfunktionalitäten
 * 
 * Diese Klasse erweitert QColor um zusätzliche Funktionen wie Farbübergänge,
 * Farbschemata und Konvertierungen zwischen verschiedenen Farbräumen.
 */
class Color : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param parent Parent-Objekt
     */
    explicit Color(QObject *parent = nullptr);
    
    /**
     * @brief Konstruktor mit QColor
     * @param color QColor-Objekt
     * @param parent Parent-Objekt
     */
    explicit Color(const QColor &color, QObject *parent = nullptr);
    
    /**
     * @brief Konstruktor mit RGB-Werten
     * @param r Rot-Komponente (0-255)
     * @param g Grün-Komponente (0-255)
     * @param b Blau-Komponente (0-255)
     * @param parent Parent-Objekt
     */
    Color(int r, int g, int b, QObject *parent = nullptr);
    
    /**
     * @brief Gibt die QColor zurück
     * @return QColor-Objekt
     */
    QColor toQColor() const;
    
    /**
     * @brief Setzt die Farbe
     * @param color QColor-Objekt
     */
    void setColor(const QColor &color);
    
    /**
     * @brief Setzt die Farbe mit RGB-Werten
     * @param r Rot-Komponente (0-255)
     * @param g Grün-Komponente (0-255)
     * @param b Blau-Komponente (0-255)
     */
    void setRGB(int r, int g, int b);
    
    /**
     * @brief Setzt die Farbe mit HSV-Werten
     * @param h Farbton (0-359)
     * @param s Sättigung (0-255)
     * @param v Hellwert (0-255)
     */
    void setHSV(int h, int s, int v);
    
    /**
     * @brief Gibt die Rot-Komponente zurück
     * @return Rot-Komponente (0-255)
     */
    int red() const;
    
    /**
     * @brief Gibt die Grün-Komponente zurück
     * @return Grün-Komponente (0-255)
     */
    int green() const;
    
    /**
     * @brief Gibt die Blau-Komponente zurück
     * @return Blau-Komponente (0-255)
     */
    int blue() const;
    
    /**
     * @brief Gibt den Farbton zurück
     * @return Farbton (0-359)
     */
    int hue() const;
    
    /**
     * @brief Gibt die Sättigung zurück
     * @return Sättigung (0-255)
     */
    int saturation() const;
    
    /**
     * @brief Gibt den Hellwert zurück
     * @return Hellwert (0-255)
     */
    int value() const;
    
    /**
     * @brief Interpoliert zwischen zwei Farben
     * @param color1 Erste Farbe
     * @param color2 Zweite Farbe
     * @param ratio Verhältnis (0.0 - 1.0)
     * @return Interpolierte Farbe
     */
    static QColor interpolate(const QColor &color1, const QColor &color2, qreal ratio);
    
    /**
     * @brief Erzeugt eine Farbe basierend auf einem Temperaturwert
     * @param temperature Temperatur (0-100)
     * @return Farbe (blau für kalt, rot für heiß)
     */
    static QColor fromTemperature(int temperature);
    
    /**
     * @brief Gibt eine benannte Farbe zurück
     * @param name Name der Farbe
     * @return QColor-Objekt
     */
    static QColor fromName(const QString &name);
    
    /**
     * @brief Gibt eine Liste aller verfügbaren Farbnamen zurück
     * @return Liste von Farbnamen
     */
    static QStringList availableColorNames();

signals:
    /**
     * @brief Signal, das bei Farbänderung ausgelöst wird
     * @param color Die neue Farbe
     */
    void colorChanged(const QColor &color);

private:
    QColor m_color;
    static QMap<QString, QColor> m_namedColors;
};
