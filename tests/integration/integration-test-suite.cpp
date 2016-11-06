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
	QDir().mkpath("tests/resources/sites/" + site + "/" + source);
	QFile::remove("tests/resources/sites/" + site +"/model.xml");
	QFile::copy("release/sites/" + site +"/model.xml", "tests/resources/sites/" + site +"/model.xml");
	QFile::remove("tests/resources/sites/" + site +"/" + source + "/settings.ini");
	if (QFile::exists("release/sites/" + site +"/" + source + "/settings.ini"))
	{ QFile::copy("release/sites/" + site +"/" + source + "/settings.ini", "tests/resources/sites/" + site +"/" + source + "/settings.ini"); }

	QSettings settings("tests/resources/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);

	QList<Site*> sites;
	sites.append(new Site(source, new Source(&profile, "tests/resources/sites/" + site)));

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
	for (QVariant variant : variants)
	{
		QSharedPointer<Image> img = variant.value<QSharedPointer<Image>>();
		result.append(img.data());
	}
	return result;
}

QList<Tag> IntegrationTestSuite::getPageTags(QString site, QString source, QString format, QString tags)
{
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
	sites.append(new Site(source, new Source(&profile, "tests/resources/sites/" + site)));

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
}
