#ifndef PROGRAM_UPDATER_H
#define PROGRAM_UPDATER_H

#include "updater.h"


class ProgramUpdater : public Updater
{
	Q_OBJECT

	public:
		ProgramUpdater(QString baseUrl);

	public slots:
		void checkForUpdates();

	private slots:
		void checkForUpdatesDone(QNetworkReply *r);

	signals:
		void finished(QString newVersion, bool available);

	private:
		QString m_baseUrl;
};

#endif // PROGRAM_UPDATER_H
