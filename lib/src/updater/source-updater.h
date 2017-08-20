#ifndef SOURCE_UPDATER_H
#define SOURCE_UPDATER_H

#include <QNetworkReply>
#include "updater.h"


class SourceUpdater : public Updater
{
	Q_OBJECT

	public:
		SourceUpdater(QString source, QString directory, QString baseUrl);

	public slots:
		void checkForUpdates();

	private slots:
		void checkForUpdatesDone();

	signals:
		void finished(QString source, bool isNew);

	private:
		QString m_source;
		QString m_directory;
		QString m_baseUrl;
		QNetworkReply *m_checkForUpdatesReply;
};

#endif // SOURCE_UPDATER_H
