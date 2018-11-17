#ifndef SOURCE_H
#define SOURCE_H

#include <QJSValue>
#include <QList>
#include <QObject>
#include <QString>
#include "tags/tag-name-format.h"
#include "updater/source-updater.h"


class Api;
class Profile;
class QJSEngine;
class QMutex;
class Site;

class Source : public QObject
{
	Q_OBJECT

	public:
		explicit Source(Profile *profile, const QString &dir);
		~Source() override;

		// Getters
		QString getName() const;
		QString getPath() const;
		const QList<Site*> &getSites() const;
		const QList<Api*> &getApis() const;
		Api *getApi(const QString &name) const;
		Profile *getProfile() const;
		const SourceUpdater &getUpdater() const;

	protected:
		QJSEngine *jsEngine();
		QMutex *jsEngineMutex();

	private:
		QString m_dir;
		QString m_diskName;
		QString m_name;
		QList<Site*> m_sites;
		QList<Api*> m_apis;
		Profile *m_profile;
		SourceUpdater m_updater;
		TagNameFormat m_tagNameFormat;
		QJSValue m_jsSource;
};

#endif // SOURCE_H
