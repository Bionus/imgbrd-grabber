#ifndef SOURCE_H
#define SOURCE_H

#include <QString>
#include <QList>
#include "updater/source-updater.h"
#include "tags/tag-name-format.h"


class Site;
class Api;
class Profile;

class Source : public QObject
{
	Q_OBJECT

	public:
		explicit Source(Profile *profile, QString dir);
		~Source() override;

		// Getters
		QString getName() const;
		QString getPath() const;
		QList<Site*> getSites() const;
		QList<Api*> getApis() const;
		Api *getApi(QString name) const;
		Profile *getProfile() const;
		const SourceUpdater &getUpdater() const;

		// Get an unique pointer to the list of all sources
		static QList<Source*> *getAllSources(Profile *profile);

	private:
		QString m_dir;
		QString m_name;
		QList<Site*> m_sites;
		QList<Api*> m_apis;
		Profile *m_profile;
		SourceUpdater m_updater;
		TagNameFormat m_tagNameFormat;
};

#endif // SOURCE_H
