#ifndef SOURCE_H
#define SOURCE_H

#include <QString>
#include <QList>
#include <QMap>
#include <QNetworkReply>
#include "api.h"



class Site;

class Source : public QObject
{
	Q_OBJECT

	public:
		Source(QString dir);
		~Source();

		// Getters
		QString getName() const;
		QString getPath() const;
		QList<Site*> getSites() const;
		QList<Api*> getApis() const;
		Api* getApi(QString name) const;
		QString getUpdateVersion() const;

		// Get an unique pointer to the list of all sources
		static QList<Source*> *getAllSources();

	public slots:
		// Check if a more recent model.xml exists for this source
		void checkForUpdates(QString baseUrl);
		void checkForUpdatesDone();

	signals:
		void checkForUpdatesFinished(Source*);

	private:
		QString m_dir;
		QString m_name;
		QList<Site*> m_sites;
		QList<Api*> m_apis;

		// Check for updates
		QNetworkAccessManager *m_manager;
		QNetworkReply *m_updateReply;
		QString m_updateVersion;
};

#endif // SOURCE_H
