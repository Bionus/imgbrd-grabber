#ifndef MD5_FIX_WORKER_H
#define MD5_FIX_WORKER_H

#include <QObject>


class Md5FixWorker : public QObject
{
	Q_OBJECT

	public slots:
		void doWork(const QString &dir, const QString &filename, const QStringList &suffixes, bool force);

	signals:
		void maximumSet(int max);
		void valueSet(int value);
		void md5Calculated(const QString &md5, const QString &path);
		void finished(int loadedCount);
};

#endif // MD5_FIX_WORKER_H
