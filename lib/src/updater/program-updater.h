#ifndef PROGRAM_UPDATER_H
#define PROGRAM_UPDATER_H

#include <QByteArray>
#include <QString>
#include <QUrl>
#include "updater/updater.h"


class NetworkReply;

class ProgramUpdater : public Updater
{
	Q_OBJECT

	public:
		ProgramUpdater();
		explicit ProgramUpdater(QString baseUrl);
		QUrl latestUrl() const;

	public slots:
		void checkForUpdates() const override;
		void downloadUpdate();

	private slots:
		void checkForUpdatesDone();
		void downloadDone();

	signals:
		void finished(const QString &newVersion, bool available, const QString &changelog);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadFinished(const QString &path);

	private:
		QString m_baseUrl;
		NetworkReply *m_downloadReply;
		QByteArray m_source;
		QString m_newVersion;
		QString m_updateFilename;
};

#endif // PROGRAM_UPDATER_H
