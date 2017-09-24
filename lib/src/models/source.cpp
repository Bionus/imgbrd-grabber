#include "source.h"
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include "site.h"
#include "api.h"
#include "profile.h"
#include "functions.h"


QList<Source*> *g_allSources = Q_NULLPTR;

#if defined NIGHTLY || defined QT_DEBUG
	const QString updaterBaseUrl = "https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/release/sites";
#else
	const QString updaterBaseUrl = "https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites";
#endif

Source::Source(Profile *profile, QString dir)
	: m_dir(dir), m_name(QFileInfo(dir).fileName()), m_profile(profile), m_updater(m_name, m_dir, updaterBaseUrl)
{
	// Load XML details for this source from its model file

	QFile file(m_dir + "/model.xml");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString fileContents = file.readAll();
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(fileContents, false, &errorMsg, &errorLine, &errorColumn))
		{ log(QString("Error parsing XML file: %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Logger::Error); }
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
			{ log(QString("No valid source has been found in the model.xml file from %1.").arg(m_name)); }
		}

		file.close();
	}
	else
	{ log(QString("Impossible to open the model file '%1'").arg(m_dir + "/model.xml")); }

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
	{ log(QString("No site for source %1").arg(m_name)); }
}

Source::~Source()
{
	qDeleteAll(m_apis);
	qDeleteAll(m_sites);
}


QString Source::getName() const 		{ return m_name;		}
QString Source::getPath() const 		{ return m_dir;			}
QList<Site*> Source::getSites() const	{ return m_sites;		}
QList<Api*> Source::getApis() const		{ return m_apis;		}
Profile *Source::getProfile() const		{ return m_profile;		}
SourceUpdater *Source::getUpdater() 	{ return &m_updater;	}

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
