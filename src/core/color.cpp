#include "core/color.h"

// Initialisierung der statischen Variablen
QMap<QString, QColor> Color::m_namedColors = {
    {"Rot", QColor(255, 0, 0)},
    {"Grün", QColor(0, 255, 0)},
    {"Blau", QColor(0, 0, 255)},
    {"Gelb", QColor(255, 255, 0)},
    {"Cyan", QColor(0, 255, 255)},
    {"Magenta", QColor(255, 0, 255)},
    {"Weiß", QColor(255, 255, 255)},
    {"Schwarz", QColor(0, 0, 0)},
    {"Orange", QColor(255, 165, 0)},
    {"Lila", QColor(128, 0, 128)},
    {"Pink", QColor(255, 192, 203)},
    {"Türkis", QColor(64, 224, 208)},
    {"Braun", QColor(165, 42, 42)},
    {"Grau", QColor(128, 128, 128)}
};

Color::Color(QObject *parent)
    : QObject(parent)
    , m_color(Qt::white)
{
}

Color::Color(const QColor &color, QObject *parent)
    : QObject(parent)
    , m_color(color)
{
}

Color::Color(int r, int g, int b, QObject *parent)
    : QObject(parent)
    , m_color(r, g, b)
{
}

QColor Color::toQColor() const
{
    return m_color;
}

void Color::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged(m_color);
    }
}

void Color::setRGB(int r, int g, int b)
{
    QColor newColor(r, g, b);
    if (m_color != newColor) {
        m_color = newColor;
        emit colorChanged(m_color);
    }
}

void Color::setHSV(int h, int s, int v)
{
    QColor newColor;
    newColor.setHsv(h, s, v);
    if (m_color != newColor) {
        m_color = newColor;
        emit colorChanged(m_color);
    }
}

int Color::red() const
{
    return m_color.red();
}

int Color::green() const
{
    return m_color.green();
}

int Color::blue() const
{
    return m_color.blue();
}

int Color::hue() const
{
    return m_color.hue();
}

int Color::saturation() const
{
    return m_color.saturation();
}

int Color::value() const
{
    return m_color.value();
}

QColor Color::interpolate(const QColor &color1, const QColor &color2, qreal ratio)
{
    if (ratio <= 0.0) return color1;
    if (ratio >= 1.0) return color2;
    
    int r = qRound(color1.red() * (1.0 - ratio) + color2.red() * ratio);
    int g = qRound(color1.green() * (1.0 - ratio) + color2.green() * ratio);
    int b = qRound(color1.blue() * (1.0 - ratio) + color2.blue() * ratio);
    
    return QColor(r, g, b);
}

QColor Color::fromTemperature(int temperature)
{
    // Temperatur auf 0-100 begrenzen
    temperature = qBound(0, temperature, 100);
    
    // Blau (kalt) bis Rot (heiß)
    if (temperature < 50) {
        // Blau bis Gelb
        return interpolate(QColor(0, 0, 255), QColor(255, 255, 0), temperature / 50.0);
    } else {
        // Gelb bis Rot
        return interpolate(QColor(255, 255, 0), QColor(255, 0, 0), (temperature - 50) / 50.0);
    }
}

QColor Color::fromName(const QString &name)
{
    if (m_namedColors.contains(name)) {
        return m_namedColors[name];
    }
    
    // Standardfarbe zurückgeben, wenn der Name nicht gefunden wurde
    return Qt::white;
}

QStringList Color::availableColorNames()
{
    return m_namedColors.keys();
}
