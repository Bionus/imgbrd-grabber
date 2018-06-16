#include "mixed-settings.h"
#include "functions.h"


MixedSettings::MixedSettings(const QList<QSettings*> &settings)
	: m_settings(settings)
{}

MixedSettings::~MixedSettings()
{
	for (QSettings *setting : qAsConst(m_settings))
		setting->deleteLater();
}


QVariant MixedSettings::value(const QString &key, const QVariant &defaultValue) const
{
	for (QSettings *setting : qAsConst(m_settings))
	{
		QVariant val = setting->value(key);
		if (val.isValid())
			return val;
	}
	return defaultValue;
}

void MixedSettings::setValue(const QString &key, const QVariant &value, const QVariant &defaultValue)
{
	if (m_settings.isEmpty())
		return;

	// If the parent setting already have this value set
	if (m_settings.count() > 1)
	{
		QVariant parent = m_settings[1]->value(key);
		if (parent.isValid())
		{
			if (parent == value)
			{
				m_settings[0]->remove(key);
				return;
			}

			// Override parent
			m_settings[0]->setValue(key, value);
			return;
		}
	}

	// No valid parent value found
	if (value == defaultValue)
	{
		m_settings.first()->remove(key);
		return;
	}

	// Override default
	m_settings[0]->setValue(key, value);
}

QStringList MixedSettings::childKeys() const
{
	QStringList keys;
	for (QSettings *setting : qAsConst(m_settings))
		keys.append(setting->childKeys());
	return keys;
}

void MixedSettings::beginGroup(const QString &prefix)
{
	for (QSettings *setting : qAsConst(m_settings))
		setting->beginGroup(prefix);
}

void MixedSettings::endGroup()
{
	for (QSettings *setting : qAsConst( m_settings))
		setting->endGroup();
}

void MixedSettings::sync()
{
	for (QSettings *setting : qAsConst(m_settings))
		setting->sync();
}
