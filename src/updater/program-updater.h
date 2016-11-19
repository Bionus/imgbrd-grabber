#ifndef PROGRAM_UPDATER_H
#define PROGRAM_UPDATER_H

#include "updater.h"
#include <QNetworkReply>


class ProgramUpdater : public Updater
{
	Q_OBJECT

	public:
		ProgramUpdater();
		ProgramUpdater(QString baseUrl);

	public slots:
		void checkForUpdates();
		QNetworkReply *downloadUpdate();

	private slots:
		void checkForUpdatesDone();

	signals:
		void finished(QString newVersion, bool available, QString changelog);

	private:
		QString m_baseUrl;
		QNetworkReply *m_checkForUpdatesReply;
		QString m_source;
};

#endif // PROGRAM_UPDATER_H
