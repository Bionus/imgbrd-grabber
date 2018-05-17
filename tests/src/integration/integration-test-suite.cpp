#include "integration-test-suite.h"
#include <QStringList>
#include <QtTest>
#include "custom-network-access-manager.h"
#include "functions.h"
#include "tags/tag-api.h"


void IntegrationTestSuite::initTestCase()
{
	Logger::getInstance().setLogFile("tests/test_log.log");

	m_downloader = nullptr;
	m_profile = nullptr;
	m_source = nullptr;
	m_site = nullptr;
}

QList<Image*> IntegrationTestSuite::getImages(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file, bool javaScript)
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

	m_profile = new Profile("tests/resources/");
	bool oldJavaScript = m_profile->getSettings()->value("enableJsModels", true).toBool();
	m_profile->getSettings()->setValue("enableJsModels", javaScript);
	m_source = new Source(m_profile, "tests/resources/sites/" + site);
	m_profile->getSettings()->setValue("enableJsModels", oldJavaScript);

	QList<Site*> sites;
	m_site = new Site(source, m_source);
	m_site->setAutoLogin(false);
	sites.append(m_site);

	QList<Image*> result;
	m_downloader = new Downloader(m_profile,
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
	result.reserve(variants.count());
	for (const QVariant &variant : variants)
	{
		QSharedPointer<Image> img = variant.value<QSharedPointer<Image>>();
		result.append(img.data());
	}
	return result;
}

QList<Tag> IntegrationTestSuite::getPageTags(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file, bool javaScript)
{
	setupSource(site);
	setupSite(site, source);

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

	m_profile = new Profile("tests/resources/");
	bool oldJavaScript = m_profile->getSettings()->value("enableJsModels", true).toBool();
	m_profile->getSettings()->setValue("enableJsModels", javaScript);
	m_source = new Source(m_profile, "tests/resources/sites/" + site);
	m_profile->getSettings()->setValue("enableJsModels", oldJavaScript);

	QList<Site*> sites;
	m_site = new Site(source, m_source);
	m_site->setAutoLogin(false);
	sites.append(m_site);

	QList<Tag> result;
	m_downloader = new Downloader(m_profile,
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
	result.reserve(variants.count());
	for (const QVariant &variant : variants)
	{
		Tag tag = variant.value<Tag>();
		result.append(tag);
	}
	return result;
}

QList<Tag> IntegrationTestSuite::getTags(const QString &site, const QString &source, const QString &format, const QString &file, bool javaScript)
{
	setupSource(site);
	setupSite(site, source);

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

	m_profile = new Profile("tests/resources/");
	bool oldJavaScript = m_profile->getSettings()->value("enableJsModels", true).toBool();
	m_profile->getSettings()->setValue("enableJsModels", javaScript);
	m_source = new Source(m_profile, "tests/resources/sites/" + site);
	m_profile->getSettings()->setValue("enableJsModels", oldJavaScript);

	m_site = new Site(source, m_source);
	m_site->setAutoLogin(false);

	QList<Tag> result;
	TagApi tagApi(m_profile, m_site, m_site->getApis().first(), 1, 100);

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
	if (m_profile != nullptr)
	{
		delete m_profile;
		m_profile = nullptr;
	}
	if (m_source != nullptr)
	{
		delete m_source;
		m_source = nullptr;
	}
	if (m_site != nullptr)
	{
		delete m_site;
		m_site = nullptr;
	}

	for (const QString &file : m_filesToRemove)
	{ QFile(file).remove(); }
	m_filesToRemove.clear();
}
