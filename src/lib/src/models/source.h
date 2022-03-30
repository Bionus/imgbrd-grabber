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
class SourceEngine;

/**
 * Represents a Grabber source, an engine and the associated settings.
 *
 * @see https://bionus.github.io/imgbrd-grabber/docs/sites/source.html
 */
class Source : public QObject
{
	Q_OBJECT

	public:
		/**
		 * Create a source for a given directory.
		 *
		 * It must at least contain a `model.js` file. Additional files are `sites.txt`, which contain the list of
		 * active sites using that source, `supported.txt`, containing a semi-exhaustive list of sites that this source
		 * supports, and `icon.png`, an icon for this source.
		 *
		 * @see https://bionus.github.io/imgbrd-grabber/docs/sites/source.html
		 *
		 * @param dir The directory to load the source from.
		 */
		explicit Source(const ReadWritePath &dir);
		~Source() override;

		/**
		 * A source is considered valid if the engine could be loaded and the engine itself is also valid.
		 * @see SourceEngine::isValid
		 * @return Whether this source is valid.
		 */
		bool isValid() const;

		// Getters
		const QString &getName() const;
		ReadWritePath getPath() const;
		const QStringList &getSites() const;
		const QStringList &getSupportedSites() const;
		SourceEngine *getEngine() const;
		const QList<Api*> &getApis() const;
		Api *getApi(const QString &name) const;
		const QMap<QString, Auth*> &getAuths() const;
		const SourceUpdater &getUpdater() const;
		const QStringList &getAdditionalTokens() const;

		// Site management
		bool addSite(const QString &site);
		bool removeSite(const QString &site);
		bool syncSites();

	private:
		ReadWritePath m_dir;
		QString m_diskName;
		SourceEngine *m_sourceEngine = nullptr;
		QStringList m_sites;
		QStringList m_supportedSites;
		SourceUpdater m_updater;
};

#endif // SOURCE_H
