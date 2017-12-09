#ifndef SOURCE_H
#define SOURCE_H

#include <QList>
#include <QString>
#include "tags/tag-name-format.h"
#include "updater/source-updater.h"


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
