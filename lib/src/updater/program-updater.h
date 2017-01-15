#ifndef PROGRAM_UPDATER_H
#define PROGRAM_UPDATER_H

#include <QNetworkReply>
#include "updater.h"


class ProgramUpdater : public Updater
{
	Q_OBJECT

	public:
		ProgramUpdater();
		ProgramUpdater(QString baseUrl);

	public slots:
		void checkForUpdates();
		void downloadUpdate();

	private slots:
		void checkForUpdatesDone();
		void downloadDone();

	signals:
		void finished(QString newVersion, bool available, QString changelog);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadFinished(QString path);

	private:
		QString m_baseUrl;
		QNetworkReply *m_checkForUpdatesReply;
		QNetworkReply *m_downloadReply;
		QString m_source;
		QString m_newVersion;
		QString m_updateFilename;
};

#endif // PROGRAM_UPDATER_H
