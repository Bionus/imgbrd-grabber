#include "settings.h"
#include <QJSValue>
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

void Settings::setValue(const QString &key, const QJSValue &value, const QVariant &defaultValue)
{
	m_settings->setValue(key, value.toVariant(), defaultValue);
}

void Settings::remove(const QString &key)
{
	m_settings->remove(key);
}

QStringList Settings::childKeys(const QString &parent)
{
	if (!parent.isEmpty()) {
		m_settings->beginGroup(parent);
	}
	QStringList ret = m_settings->childKeys();
	if (!parent.isEmpty()) {
		m_settings->endGroup();
	}
	return ret;
}
