#include "models/source.h"
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>
#include <QMutex>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/auth-hash-field.h"
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth1-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "functions.h"
#include "javascript-source-engine.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/api/javascript-api.h"
#include "models/site.h"
#include "js-helpers.h"


QString getUpdaterBaseUrl()
{
	#if defined NIGHTLY || defined QT_DEBUG
		return QStringLiteral("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/src/sites");
	#else
		return QStringLiteral("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/src/sites");
	#endif
}

QStringList readFileLines(const QString &path)
{
	QStringList ret;
	QFile f(path);
	if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		while (!f.atEnd()) {
			const QString line = f.readLine().trimmed();
			if (line.isEmpty()) {
				continue;
			}
			ret.append(line);
		}
	}
	return ret;
}

Source::Source(const ReadWritePath &dir)
	: m_dir(dir), m_diskName(QFileInfo(dir.readPath()).fileName()), m_updater(m_diskName, m_dir, getUpdaterBaseUrl())
{
	// Javascript engine
	const QString modelPath = m_dir.readPath("model.js");
	if (QFile::exists(modelPath)) {
		log(QStringLiteral("Using Javascript engine for '%1'").arg(m_diskName), Logger::Debug);
		auto *javascriptSource = new JavaScriptSourceEngine(modelPath, m_dir.readPath("../helper.js"), this);
		if (javascriptSource->isValid()) {
			m_sourceEngine = javascriptSource;
		} else {
			javascriptSource->deleteLater();
		}
	} else {
		log(QStringLiteral("Javascript 'model.js' file not found for '%1' in `%2`").arg(m_diskName, modelPath), Logger::Warning);
	}

	// Get the list of all sites pertaining to this source
	m_sites = readFileLines(m_dir.readPath("sites.txt"));
	if (m_sites.isEmpty()) {
		log(QStringLiteral("No site for source '%1'").arg(m_diskName), Logger::Debug);
	}

	// Get the list of all supported sites for this source
	m_supportedSites = readFileLines(m_dir.readPath("supported.txt"));
}

Source::~Source()
{
	if (m_sourceEngine != nullptr) {
		m_sourceEngine->deleteLater();
	}
}


bool Source::addSite(const QString &site)
{
	m_sites.append(site);
	m_sites.removeDuplicates();
	m_sites.sort();

	return syncSites();
}

bool Source::removeSite(const QString &site)
{
	m_sites.removeAll(site);

	return syncSites();
}

bool Source::syncSites()
{
	return writeFile(m_dir.writePath("sites.txt"), m_sites.join("\r\n").toLatin1());
}


const QString &Source::getName() const { return m_sourceEngine->getName(); }
ReadWritePath Source::getPath() const { return m_dir; }
const QStringList &Source::getSites() const { return m_sites; }
const QStringList &Source::getSupportedSites() const { return m_supportedSites; }
const QList<Api*> &Source::getApis() const { return m_sourceEngine->getApis(); }
Api *Source::getApi(const QString &name) const { return m_sourceEngine->getApi(name); }
const SourceUpdater &Source::getUpdater() const { return m_updater; }
const QStringList &Source::getAdditionalTokens() const { return m_sourceEngine->getAdditionalTokens(); }
const QMap<QString, Auth*> &Source::getAuths() const { return m_sourceEngine->getAuths(); }
