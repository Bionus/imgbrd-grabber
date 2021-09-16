#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>


class MixedSettings;
class QSettings;

class Settings : public QObject
{
	Q_OBJECT

	public:
		explicit Settings(QSettings *settings, QObject *parent = nullptr);
		explicit Settings(MixedSettings *settings, QObject *parent = nullptr);

		Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
		Q_INVOKABLE void setValue(const QString &key, const QVariant &value, const QVariant &defaultValue = {});
		Q_INVOKABLE void remove(const QString &key);
		Q_INVOKABLE QStringList childKeys(const QString &parent = {});

	private:
		MixedSettings *m_settings;
};

#endif // SETTINGS_H
