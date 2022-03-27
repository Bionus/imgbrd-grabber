#ifndef SOURCE_H
#define SOURCE_H

#include <QJSValue>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include "tags/tag-name-format.h"
#include "updater/source-updater.h"
#include "utils/read-write-path.h"


class Api;
class Auth;
class Profile;
class QJSEngine;
class QMutex;
class Site;

class Source : public QObject
{
	Q_OBJECT

	public:
		explicit Source(const ReadWritePath &dir);
		~Source() override;

		// Getters
		QString getName() const;
		ReadWritePath getPath() const;
		const QStringList &getSites() const;
		const QStringList &getSupportedSites() const;
		const QList<Api*> &getApis() const;
		Api *getApi(const QString &name) const;
		const QMap<QString, Auth*> &getAuths() const;
		const SourceUpdater &getUpdater() const;
		const QStringList &getAdditionalTokens() const;

		// Site management
		bool addSite(const QString &site);
		bool removeSite(const QString &site);

	protected:
		QJSEngine *jsEngine();
		QMutex *jsEngineMutex();

	private:
		ReadWritePath m_dir;
		QString m_diskName;
		QString m_name;
		QStringList m_sites;
		QStringList m_supportedSites;
		QList<Api*> m_apis;
		QMap<QString, Auth*> m_auths;
		QStringList m_additionalTokens;
		SourceUpdater m_updater;
		TagNameFormat m_tagNameFormat;
		QJSValue m_jsSource;
};

#endif // SOURCE_H
