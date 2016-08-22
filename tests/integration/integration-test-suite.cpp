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
	m_settings = new QSettings("tests/test_settings.ini", QSettings::IniFormat);
    QSettings settings("release/sites/" + site +"/" + source + "/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
    settings.setValue("sources/source_1", format);

	QList<Site*> sites;
    sites.append(new Site(m_settings, "release/sites/" + site, source));

    QList<Image*> images;
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
        return images;

    // Get results
    QList<QVariant> arguments = spy.takeFirst();
    QVariantList variants = arguments.at(0).value<QVariantList>();

    // Convert results
    for (QVariant variant : variants)
    {
        Image *img = variant.value<Image*>();
        images.append(img);
    }
    return images;
}

void IntegrationTestSuite::cleanup()
{
	m_downloader->deleteLater();
	m_settings->deleteLater();
}
