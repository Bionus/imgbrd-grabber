#ifndef SOURCE_UPDATER_H
#define SOURCE_UPDATER_H

#include <QNetworkReply>
#include "updater.h"


class Source;

class SourceUpdater : public Updater
{
	Q_OBJECT

	public:
		SourceUpdater(Source *source, QString baseUrl);

	public slots:
		void checkForUpdates();

	private slots:
		void checkForUpdatesDone();

	signals:
		void finished(Source *source, bool isNew);

	private:
		Source *m_source;
		QString m_baseUrl;
		QNetworkReply *m_checkForUpdatesReply;
};

#endif // SOURCE_UPDATER_H
