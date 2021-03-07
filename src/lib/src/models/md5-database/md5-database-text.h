#ifndef MD5_DATABASE_TEXT_H
#define MD5_DATABASE_TEXT_H

#include "models/md5-database/md5-database.h"
#include <QMultiHash>
#include <QString>
#include <QStringList>
#include <QTimer>


class QSettings;

class Md5DatabaseText : public Md5Database
{
	Q_OBJECT

	public:
		explicit Md5DatabaseText(QString path, QSettings *settings);
		~Md5DatabaseText() override;

		void sync() override;
		void add(const QString &md5, const QString &path) override;
		void remove(const QString &md5, const QString &path = {}) override;
		int count() const override;

		const QMultiHash<QString, QString> &getAll() const;

	protected:
		QStringList paths(const QString &md5) override;

	protected slots:
		void flush();

	signals:
		void flushed();

	private:
		QString m_path;
		QMultiHash<QString, QString> m_md5s;
		QTimer m_flushTimer;
		QMultiHash<QString, QString> m_pendingAdd;
};

#endif // MD5_DATABASE_TEXT_H
