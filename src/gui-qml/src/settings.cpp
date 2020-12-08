#include "settings.h"
#include <QSettings>
#include "mixed-settings.h"


Settings::Settings(QSettings *settings, QObject *parent)
	: Settings(new MixedSettings({ settings }), parent)
{}

Settings::Settings(MixedSettings *settings, QObject *parent)
	: QObject(parent), m_settings(settings)
{}


QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings->value(key, defaultValue);
}

void Settings::setValue(const QString &key, const QVariant &value, const QVariant &defaultValue)
{
	m_settings->setValue(key, value, defaultValue);
}
