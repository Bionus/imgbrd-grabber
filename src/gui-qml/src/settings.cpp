#include "settings.h"
#include <QSettings>


Settings::Settings(QSettings *settings, QObject *parent)
    : QObject(parent), m_settings(settings)
{}


QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings->value(key, defaultValue);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    m_settings->setValue(key, value);
}
