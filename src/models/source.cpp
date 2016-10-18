#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "source.h"
#include "site.h"
#include "functions.h"



QList<Source*> *g_allSources = Q_NULLPTR;

Source::Source(Profile *profile, QString dir)
	: m_dir(dir), m_profile(profile), m_updateReply(nullptr), m_updateVersion("")
{
	// Load XML details for this source from its model file
	QFile file(m_dir + "/model.xml");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QFileInfo info(m_dir);
		m_name = info.fileName();

		QString fileContents = file.readAll();
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(fileContents, false, &errorMsg, &errorLine, &errorColumn))
		{ log(tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Error); }
		else
		{
			QDomElement docElem = doc.documentElement();
			QMap<QString, QString> details = domToMap(docElem);

			// Get the list of possible API for this Source
			QStringList possibleApis = QStringList() << "Xml" << "Json" << "Rss" << "Html";
			QStringList availableApis;
			for (QString api : possibleApis)
				if (details.contains("Urls/" + api + "/Tags"))
					availableApis.append(api);

			if (!availableApis.isEmpty())
			{
				for (QString apiName : availableApis)
				{
					Api *api = new Api(apiName, details);
					m_apis.append(api);
				}
			}
			else
			{ log(tr("Aucune source valide trouvée dans le fichier model.xml de %1.").arg(m_name)); }
		}

		file.close();
	}
	else
	{ log(tr("Impossible d'ouvrir le fichier de modèle '%1'").arg(m_dir + "/model.xml")); }

	// Get the list of all sites pertaining to this source
	QFile f(m_dir + "/sites.txt");
	if (f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!f.atEnd())
		{
			QString line = f.readLine().trimmed();
			if (line.isEmpty())
				continue;

			Site *site = new Site(line, this);
			m_sites.append(site);
		}
	}
	if (m_sites.isEmpty())
	{ log(tr("Aucun site pour la source %1").arg(m_name)); }

	m_manager = new QNetworkAccessManager(this);
	connect(m_manager, &QNetworkAccessManager::sslErrors, sslErrorHandler);
}

Source::~Source()
{
	m_manager->deleteLater();
}


/**
 * Check if an update is available for this source's model file.
 */
void Source::checkForUpdates(QString baseUrl)
{
	QUrl url(baseUrl + m_name + "/model.xml");
	QNetworkRequest request(url);

	m_updateReply = m_manager->get(request);
	connect(m_updateReply, &QNetworkReply::finished, this, &Source::checkForUpdatesDone);
}

/**
 * Called when the update check is finished.
 */
void Source::checkForUpdatesDone()
{
	QString source = m_updateReply->readAll();
	if (source.startsWith("<?xml"))
	{
		QFile current(m_dir + "/model.xml");
		if (current.open(QFile::ReadOnly))
		{
			QString compare = current.readAll();
			current.close();

			if (compare != source)
			{ m_updateVersion = VERSION; }
		}
	}

	m_updateReply->deleteLater();
	m_updateReply = nullptr;
	emit checkForUpdatesFinished(this);
}


QString Source::getName() const 			{ return m_name;			}
QString Source::getPath() const 			{ return m_dir;				}
QList<Site*> Source::getSites() const		{ return m_sites;			}
QList<Api*> Source::getApis() const			{ return m_apis;			}
QString Source::getUpdateVersion() const	{ return m_updateVersion;	}
Profile *Source::getProfile() const			{ return m_profile;			}

Api *Source::getApi(QString name) const
{
	for (Api *api : this->getApis())
		if (api->getName() == name)
			return api;
	return nullptr;
}



QList<Source*> *Source::getAllSources(Profile *profile)
{
	if (g_allSources != Q_NULLPTR)
		return g_allSources;

	QList<Source*> *sources = new QList<Source*>();
	QStringList dirs = QDir(profile->getPath() + "/sites/").entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (QString dir : dirs)
	{
		Source *source = new Source(profile, profile->getPath() + "/sites/" + dir);
		sources->append(source);
	}

	g_allSources = sources;
	return g_allSources;
}
