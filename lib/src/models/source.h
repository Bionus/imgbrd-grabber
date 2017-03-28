#ifndef SOURCE_H
#define SOURCE_H

#include <QString>
#include <QList>
#include "updater/source-updater.h"


class Site;
class Api;
class Profile;

class Source : public QObject
{
	Q_OBJECT

	public:
		Source(Profile *profile, QString dir);

		// Getters
		QString getName() const;
		QString getPath() const;
		QList<Site*> getSites() const;
		QList<Api*> getApis() const;
		Api *getApi(QString name) const;
		Profile *getProfile() const;
		SourceUpdater *getUpdater();

		// Get an unique pointer to the list of all sources
		static QList<Source*> *getAllSources(Profile *profile);

	private:
		QString m_dir;
		QString m_name;
		QList<Site*> m_sites;
		QList<Api*> m_apis;
		Profile *m_profile;
		SourceUpdater m_updater;
};

#endif // SOURCE_H
