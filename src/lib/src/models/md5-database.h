#ifndef MD5_DATABASE_H
#define MD5_DATABASE_H

#include <QMultiHash>
#include <QObject>
#include <QPair>
#include <QString>
#include <QTimer>


class QSettings;

class Md5Database : public QObject
{
	Q_OBJECT

	public:
		explicit Md5Database(QString file, QSettings *settings);
		~Md5Database() override;
		void sync();

		QPair<QString, QString> action(const QString &md5, const QString &target);
		QStringList exists(const QString &md5);
		void add(const QString &md5, const QString &path);
		void remove(const QString &md5, const QString &path = {});

	protected slots:
		void flush();

	protected:
		QPair<QString, QString> action(const QString &md5, const QStringList &paths, QString action);

	signals:
		void flushed();

	private:
		QString m_path;
		QSettings *m_settings;
		QMultiHash<QString, QString> m_md5s;
		QTimer m_flushTimer;
		QMultiHash<QString, QString> m_pendingAdd;
};

#endif // MD5_DATABASE_H
