#include <QtTest>
#include <QStringList>
#include "danbooru-test.h"
#include "models/site.h"
#include "functions.h"


void DanbooruTest::initTestCase()
{
	setLogFile("test_log.log");
}

void DanbooruTest::init()
{
	m_settings = new QSettings("test_settings.ini", QSettings::IniFormat);

	QList<Site*> sites;
	Site *site = new Site(m_settings, "../release/sites/Danbooru (2.0)", "danbooru.donmai.us");
	site->settings()->setValue("download/throttle_retry", 0);
	site->settings()->setValue("download/throttle_page", 0);
	site->settings()->setValue("download/throttle_thumbnail", 0);
	site->settings()->setValue("download/throttle_details", 0);
	sites.append(site);

	m_downloader = new Downloader(QStringList() << "rating:safe",
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
}

void DanbooruTest::cleanup()
{
	m_downloader->deleteLater();
	m_settings->deleteLater();
}


void DanbooruTest::testXml()
{
    QSignalSpy spy(m_downloader, SIGNAL(finishedImages(QList<Image*>)));
    m_downloader->getImages();

	// Wait for signal
	QVERIFY(spy.wait());

    // Get results
    QList<QVariant> arguments = spy.takeFirst();
    QVariantList variants = arguments.at(0).value<QVariantList>();

    // Convert results
    QList<Image*> images;
    QStringList md5s;
    for (QVariant variant : variants)
    {
        Image *img = variant.value<Image*>();
        images.append(img);
        md5s.append(img->md5());
    }

    // Check results
    md5s = md5s.mid(0, 3);
    QStringList expected = QStringList() << "test98bf686ca7910ee0ad48e59ba99807d3" << "e5d074c5fe05e1493372e7224d2d198f" << "1e774a4a9d080611fde61e58625e038e";
    QCOMPARE(images.count(), 20);
    QCOMPARE(md5s, expected);
}

static DanbooruTest instance;
