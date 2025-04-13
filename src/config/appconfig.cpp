#include "config/appconfig.h"
#include <QDebug>
#include <QDir>

AppConfig::AppConfig(QObject *parent)
    : QObject(parent)
    , m_settings("LuminControl", "LuminControl")
    , m_theme("Default")
    , m_startWithSystem(false)
{
    loadConfig();
}

AppConfig::~AppConfig()
{
    saveConfig();
}

void AppConfig::setTheme(const QString &themeName)
{
    m_theme = themeName;
}

QString AppConfig::getTheme() const
{
    return m_theme;
}

void AppConfig::setStartWithSystem(bool enabled)
{
    m_startWithSystem = enabled;
}

bool AppConfig::startsWithSystem() const
{
    return m_startWithSystem;
}

void AppConfig::saveConfig()
{
    m_settings.setValue("theme", m_theme);
    m_settings.setValue("startWithSystem", m_startWithSystem);
    m_settings.sync();
    
    qDebug() << "Konfiguration gespeichert";
}

void AppConfig::loadConfig()
{
    m_theme = m_settings.value("theme", "Default").toString();
    m_startWithSystem = m_settings.value("startWithSystem", false).toBool();
    
    qDebug() << "Konfiguration geladen: Theme =" << m_theme 
             << ", Start mit System =" << m_startWithSystem;
}
