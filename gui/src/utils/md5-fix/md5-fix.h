#ifndef MD5_FIX_H
#define MD5_FIX_H

#include <QDialog>
#include <QThread>
#include "md5-fix-worker.h"


namespace Ui
{
	class Md5Fix;
}


class Profile;

class Md5Fix : public QDialog
{
	Q_OBJECT

	public:
		explicit Md5Fix(Profile *profile, QWidget *parent = nullptr);
		~Md5Fix();

	private slots:
		void cancel();
		void start();

		// Worker events
		void workerMaximumSet(int max);
		void workerValueSet(int value);
		void workerMd5Calculated(const QString &md5, const QString &path);
		void workerFinished(int loadedCount);

	signals:
		void startWorker(const QString &dir, const QString &format, const QStringList &suffixes, bool force);

	private:
		Ui::Md5Fix *ui;
		Profile *m_profile;
		QThread m_thread;
		Md5FixWorker *m_worker;
};

#endif // MD5_FIX_H
