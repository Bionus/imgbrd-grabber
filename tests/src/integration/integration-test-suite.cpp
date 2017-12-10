#include <QtTest>
#include <QStringList>
#include "integration-test-suite.h"
#include "tags/tag-api.h"
#include "functions.h"


void IntegrationTestSuite::initTestCase()
{
	Logger::getInstance().setLogFile("tests/test_log.log");
	m_downloader = nullptr;
}

QList<Image*> IntegrationTestSuite::getImages(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file)
{
	setupSource(site);
	setupSite(site, source);

	// Setup network
	if (!file.isEmpty())
	{ CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/" + source + "/" + file); }

	QSettings settings("tests/resources/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);
	settings.sync();
	m_filesToRemove.append(settings.fileName());

	QList<Site*> sites;
	Site *ste = new Site(source, new Source(&profile, "tests/resources/sites/" + site));
	ste->setAutoLogin(false);
	sites.append(ste);

	QList<Image*> result;
	m_downloader = new Downloader(&profile,
								  tags.split(' '),
								  QStringList(),
								  sites,
								  1,
								  20,
								  20,
								  ".",
								  "%md5%.%ext%",
								  "",
								  "",
								  false,
								  QStringList(),
								  false,
								  0,
								  "%tag %count %type");
	m_downloader->setQuit(false);

	// Wait for downloader
	QSignalSpy spy(m_downloader, SIGNAL(finishedImages(QList<QSharedPointer<Image>>)));
	m_downloader->getImages();
	if (!spy.wait())
		return result;

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	QVariantList variants = arguments.at(0).value<QVariantList>();

	// Convert results
	for (const QVariant &variant : variants)
	{
		QSharedPointer<Image> img = variant.value<QSharedPointer<Image>>();
		result.append(img.data());
	}
	return result;
}

QList<Tag> IntegrationTestSuite::getPageTags(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file)
{
	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::copy("release/sites/" + site +"/model.xml", "tests/resources/sites/" + site +"/model.xml");
	QFile::copy("release/sites/" + site +"/" + source + "/defaults.ini", "tests/resources/sites/" + site +"/" + source + "/defaults.ini");

	QSettings settings("tests/resources/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);
	settings.sync();
	m_filesToRemove.append(settings.fileName());

	// Setup network
	if (!file.isEmpty())
	{ CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/" + source + "/" + file); }

	QList<Site*> sites;
	Site *ste = new Site(source, new Source(&profile, "tests/resources/sites/" + site));
	ste->setAutoLogin(false);
	sites.append(ste);

	QList<Tag> result;
	m_downloader = new Downloader(&profile,
								  tags.split(' '),
								  QStringList(),
								  sites,
								  1,
								  20,
								  20,
								  ".",
								  "%md5%.%ext%",
								  "",
								  "",
								  false,
								  QStringList(),
								  false,
								  0,
								  "%tag %count %type");
	m_downloader->setQuit(false);

	// Wait for downloader
	QSignalSpy spy(m_downloader, SIGNAL(finishedTags(QList<Tag>)));
	m_downloader->getPageTags();
	if (!spy.wait())
		return result;

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	QVariantList variants = arguments.at(0).value<QVariantList>();

	// Convert results
	for (const QVariant &variant : variants)
	{
		Tag tag = variant.value<Tag>();
		result.append(tag);
	}
	return result;
}

QList<Tag> IntegrationTestSuite::getTags(const QString &site, const QString &source, const QString &format, const QString &file)
{
	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::copy("release/sites/" + site +"/model.xml", "tests/resources/sites/" + site +"/model.xml");
	QFile::copy("release/sites/" + site +"/" + source + "/defaults.ini", "tests/resources/sites/" + site +"/" + source + "/defaults.ini");

	QSettings settings("tests/resources/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);
	settings.sync();
	m_filesToRemove.append(settings.fileName());

	// Setup network
	if (!file.isEmpty())
	{ CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/" + source + "/" + file); }

	Site *ste = new Site(source, new Source(&profile, "tests/resources/sites/" + site));
	ste->setAutoLogin(false);

	QList<Tag> result;
	TagApi tagApi(&profile, ste, ste->getApis().first(), 1, 100);

	// Wait for tag api
	QSignalSpy spy(&tagApi, SIGNAL(finishedLoading(TagApi*, TagApi::LoadResult)));
	tagApi.load(false);
	if (!spy.wait())
		return result;

	// Check result type
	QList<QVariant> arguments = spy.takeFirst();
	TagApi::LoadResult res = arguments.at(1).value<TagApi::LoadResult>();
	if (res != TagApi::LoadResult::Ok)
		return result;

	return tagApi.tags();
}

void IntegrationTestSuite::cleanup()
{
	if (m_downloader != nullptr)
	{
		m_downloader->deleteLater();
		m_downloader = nullptr;
	}

	for (const QString &file : m_filesToRemove)
	{ QFile(file).remove(); }
	m_filesToRemove.clear();
}
