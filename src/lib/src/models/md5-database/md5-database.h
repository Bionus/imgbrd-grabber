#ifndef MD5_DATABASE_H
#define MD5_DATABASE_H

#include <QObject>
#include <QPair>
#include <QString>


class QSettings;

class Md5Database : public QObject
{
	Q_OBJECT

	public:
		explicit Md5Database(QSettings *settings);

		QPair<QString, QString> action(const QString &md5, const QString &target);
		QStringList exists(const QString &md5);

		virtual void sync() = 0;
		virtual void add(const QString &md5, const QString &path) = 0;
		virtual void remove(const QString &md5, const QString &path = {}) = 0;
		virtual int count() const = 0;

	protected:
		QPair<QString, QString> action(const QString &md5, const QStringList &paths, QString action);
		virtual QStringList paths(const QString &md5) = 0;

	protected:
		QSettings *m_settings;
};

#endif // MD5_DATABASE_H
