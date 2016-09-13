#include <QtTest>
#include <QStringList>
#include "integration-test-suite.h"
#include "functions.h"


void IntegrationTestSuite::initTestCase()
{
	setLogFile("tests/test_log.log");
}

QList<Image*> IntegrationTestSuite::getImages(QString site, QString source, QString format, QString tags)
{
	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);

	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::remove("tests/resources/sites/" + site +"/model.xml");
	QFile::copy("release/sites/" + site +"/model.xml", "tests/resources/sites/" + site +"/model.xml");
	if (QFile::exists("release/sites/" + site +"/" + source + "/settings.ini"))
	{ QFile::copy("release/sites/" + site +"/" + source + "/settings.ini", "tests/resources/sites/" + site +"/" + source + "/settings.ini"); }
	else
	{ QFile::remove("tests/resources/sites/" + site +"/" + source + "/settings.ini"); }

	QSettings settings("tests/resources/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);

	QList<Site*> sites;
	sites.append(new Site(m_settings, "tests/resources/sites/" + site, source));

	QList<Image*> result;
	m_downloader = new Downloader(tags.split(' '),
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
	QSignalSpy spy(m_downloader, SIGNAL(finishedImages(QList<Image*>)));
	m_downloader->getImages();
	if (!spy.wait())
		return result;

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	QVariantList variants = arguments.at(0).value<QVariantList>();

	// Convert results
	for (QVariant variant : variants)
	{
		Image *img = variant.value<Image*>();
		result.append(img);
	}
	return result;
}

QList<Tag> IntegrationTestSuite::getPageTags(QString site, QString source, QString format, QString tags)
{
	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);

	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::copy("release/sites/" + site +"/model.xml", "tests/resources/sites/" + site +"/model.xml");
	QFile::copy("release/sites/" + site +"/" + source + "/settings.ini", "tests/resources/sites/" + site +"/" + source + "/settings.ini");

	QSettings settings("tests/resources/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);

	QList<Site*> sites;
	sites.append(new Site(m_settings, "tests/resources/sites/" + site, source));

	QList<Tag> result;
	m_downloader = new Downloader(tags.split(' '),
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
	for (QVariant variant : variants)
	{
		Tag tag = variant.value<Tag>();
		result.append(tag);
	}
	return result;
}

void IntegrationTestSuite::cleanup()
{
	m_downloader->deleteLater();
	m_settings->deleteLater();
}
