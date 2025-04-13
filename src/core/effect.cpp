#include "core/effect.h"
#include "core/color.h"
#include <QDebug>

// Effect Basisklasse
Effect::Effect(Type type, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_active(false)
    , m_timer(new QTimer(this))
{
}

Effect::~Effect()
{
    stop();
}

Effect::Type Effect::getType() const
{
    return m_type;
}

QString Effect::getName() const
{
    return nameFromType(m_type);
}

void Effect::setParameters(const QVariantMap &parameters)
{
    m_parameters = parameters;
}

void Effect::start()
{
    m_active = true;
}

void Effect::stop()
{
    m_active = false;
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

bool Effect::isActive() const
{
    return m_active;
}

Effect* Effect::createEffect(Type type, const QVariantMap &parameters, QObject *parent)
{
    Effect *effect = nullptr;
    
    switch (type) {
        case Static: {
            QColor color = parameters.value("color", QColor(Qt::white)).value<QColor>();
            effect = new StaticEffect(color, parent);
            break;
        }
        case Breathing: {
            QColor color = parameters.value("color", QColor(Qt::white)).value<QColor>();
            int speed = parameters.value("speed", 2000).toInt();
            effect = new BreathingEffect(color, speed, parent);
            break;
        }
        case Rainbow: {
            int speed = parameters.value("speed", 2000).toInt();
            effect = new RainbowEffect(speed, parent);
            break;
        }
        case Wave: {
            QColor color1 = parameters.value("color1", QColor(Qt::blue)).value<QColor>();
            QColor color2 = parameters.value("color2", QColor(Qt::cyan)).value<QColor>();
            int speed = parameters.value("speed", 2000).toInt();
            effect = new WaveEffect(color1, color2, speed, parent);
            break;
        }
        case Reactive: {
            QColor color = parameters.value("color", QColor(Qt::white)).value<QColor>();
            int duration = parameters.value("duration", 500).toInt();
            effect = new ReactiveEffect(color, duration, parent);
            break;
        }
        default:
            qWarning() << "Unbekannter Effekttyp:" << type;
            effect = new StaticEffect(Qt::white, parent);
            break;
    }
    
    if (effect) {
        effect->setParameters(parameters);
    }
    
    return effect;
}

Effect::Type Effect::typeFromName(const QString &name)
{
    if (name == "Statisch") return Static;
    if (name == "Atmen") return Breathing;
    if (name == "Regenbogen") return Rainbow;
    if (name == "Welle") return Wave;
    if (name == "Reaktiv") return Reactive;
    
    // Standardwert zurückgeben, wenn der Name nicht erkannt wurde
    return Static;
}

QString Effect::nameFromType(Type type)
{
    switch (type) {
        case Static: return "Statisch";
        case Breathing: return "Atmen";
        case Rainbow: return "Regenbogen";
        case Wave: return "Welle";
        case Reactive: return "Reaktiv";
        default: return "Unbekannt";
    }
}

// StaticEffect
StaticEffect::StaticEffect(const QColor &color, QObject *parent)
    : Effect(Static, parent)
    , m_color(color)
{
}

QColor StaticEffect::getCurrentColor() const
{
    return m_color;
}

void StaticEffect::setParameters(const QVariantMap &parameters)
{
    Effect::setParameters(parameters);
    
    if (parameters.contains("color")) {
        m_color = parameters["color"].value<QColor>();
        emit colorChanged(m_color);
    }
}

// BreathingEffect
BreathingEffect::BreathingEffect(const QColor &color, int speed, QObject *parent)
    : Effect(Breathing, parent)
    , m_color(color)
    , m_speed(speed)
    , m_intensity(0.0)
    , m_increasing(true)
{
    connect(m_timer, &QTimer::timeout, this, &BreathingEffect::update);
}

QColor BreathingEffect::getCurrentColor() const
{
    // Farbe mit aktueller Intensität berechnen
    int h, s, v;
    m_color.getHsv(&h, &s, &v);
    
    // Helligkeit basierend auf Intensität anpassen
    int newV = qRound(v * m_intensity);
    
    QColor currentColor;
    currentColor.setHsv(h, s, newV);
    return currentColor;
}

void BreathingEffect::setParameters(const QVariantMap &parameters)
{
    Effect::setParameters(parameters);
    
    if (parameters.contains("color")) {
        m_color = parameters["color"].value<QColor>();
    }
    
    if (parameters.contains("speed")) {
        m_speed = parameters["speed"].toInt();
        if (m_active) {
            m_timer->setInterval(m_speed / 50);
        }
    }
}

void BreathingEffect::start()
{
    Effect::start();
    m_intensity = 0.1;
    m_increasing = true;
    m_timer->setInterval(m_speed / 50);
    m_timer->start();
}

void BreathingEffect::update()
{
    // Intensität erhöhen oder verringern
    if (m_increasing) {
        m_intensity += 0.02;
        if (m_intensity >= 1.0) {
            m_intensity = 1.0;
            m_increasing = false;
        }
    } else {
        m_intensity -= 0.02;
        if (m_intensity <= 0.1) {
            m_intensity = 0.1;
            m_increasing = true;
        }
    }
    
    emit colorChanged(getCurrentColor());
}

// RainbowEffect
RainbowEffect::RainbowEffect(int speed, QObject *parent)
    : Effect(Rainbow, parent)
    , m_speed(speed)
    , m_hue(0)
{
    connect(m_timer, &QTimer::timeout, this, &RainbowEffect::update);
}

QColor RainbowEffect::getCurrentColor() const
{
    QColor color;
    color.setHsv(m_hue, 255, 255);
    return color;
}

void RainbowEffect::setParameters(const QVariantMap &parameters)
{
    Effect::setParameters(parameters);
    
    if (parameters.contains("speed")) {
        m_speed = parameters["speed"].toInt();
        if (m_active) {
            m_timer->setInterval(m_speed / 360);
        }
    }
}

void RainbowEffect::start()
{
    Effect::start();
    m_hue = 0;
    m_timer->setInterval(m_speed / 360);
    m_timer->start();
}

void RainbowEffect::update()
{
    // Farbton im Regenbogen durchlaufen
    m_hue = (m_hue + 1) % 360;
    emit colorChanged(getCurrentColor());
}

// WaveEffect
WaveEffect::WaveEffect(const QColor &color1, const QColor &color2, int speed, QObject *parent)
    : Effect(Wave, parent)
    , m_color1(color1)
    , m_color2(color2)
    , m_speed(speed)
    , m_position(0.0)
    , m_forward(true)
{
    connect(m_timer, &QTimer::timeout, this, &WaveEffect::update);
}

QColor WaveEffect::getCurrentColor() const
{
    return Color::interpolate(m_color1, m_color2, m_position);
}

void WaveEffect::setParameters(const QVariantMap &parameters)
{
    Effect::setParameters(parameters);
    
    if (parameters.contains("color1")) {
        m_color1 = parameters["color1"].value<QColor>();
    }
    
    if (parameters.contains("color2")) {
        m_color2 = parameters["color2"].value<QColor>();
    }
    
    if (parameters.contains("speed")) {
        m_speed = parameters["speed"].toInt();
        if (m_active) {
            m_timer->setInterval(m_speed / 100);
        }
    }
}

void WaveEffect::start()
{
    Effect::start();
    m_position = 0.0;
    m_forward = true;
    m_timer->setInterval(m_speed / 100);
    m_timer->start();
}

void WaveEffect::update()
{
    // Position im Farbverlauf aktualisieren
    if (m_forward) {
        m_position += 0.01;
        if (m_position >= 1.0) {
            m_position = 1.0;
            m_forward = false;
        }
    } else {
        m_position -= 0.01;
        if (m_position <= 0.0) {
            m_position = 0.0;
            m_forward = true;
        }
    }
    
    emit colorChanged(getCurrentColor());
}

// ReactiveEffect
ReactiveEffect::ReactiveEffect(const QColor &color, int duration, QObject *parent)
    : Effect(Reactive, parent)
    , m_color(color)
    , m_baseColor(Qt::black)
    , m_duration(duration)
    , m_intensity(0.0)
{
    connect(m_timer, &QTimer::timeout, this, &ReactiveEffect::update);
}

QColor ReactiveEffect::getCurrentColor() const
{
    return Color::interpolate(m_baseColor, m_color, m_intensity);
}

void ReactiveEffect::setParameters(const QVariantMap &parameters)
{
    Effect::setParameters(parameters);
    
    if (parameters.contains("color")) {
        m_color = parameters["color"].value<QColor>();
    }
    
    if (parameters.contains("baseColor")) {
        m_baseColor = parameters["baseColor"].value<QColor>();
    }
    
    if (parameters.contains("duration")) {
        m_duration = parameters["duration"].toInt();
    }
}

void ReactiveEffect::start()
{
    Effect::start();
    m_intensity = 0.0;
    m_timer->setInterval(m_duration / 50);
}

void ReactiveEffect::trigger()
{
    if (!m_active) {
        start();
    }
    
    m_intensity = 1.0;
    emit colorChanged(getCurrentColor());
    
    if (!m_timer->isActive()) {
        m_timer->start();
    }
}

void ReactiveEffect::update()
{
    // Intensität verringern
    m_intensity -= 0.02;
    
    if (m_intensity <= 0.0) {
        m_intensity = 0.0;
        m_timer->stop();
    }
    
    emit colorChanged(getCurrentColor());
}
