#ifndef MIXED_SETTINGS_H
#define MIXED_SETTINGS_H

#include <QSettings>


class MixedSettings : public QObject
{
	Q_OBJECT

	public:
		explicit MixedSettings(const QList<QSettings*> &settings);
		~MixedSettings() override;

		QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
		void setValue(const QString &key, const QVariant &value, const QVariant &defaultValue = QVariant());
		QStringList childKeys() const;
		void beginGroup(const QString &prefix);
		void endGroup();
		void sync();

	private:
		QList<QSettings*> m_settings;
};

#endif // MIXED_SETTINGS_H
