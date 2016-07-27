#include <QtTest>
#include <QStringList>
#include "danbooru-test.h"
#include "models/site.h"
#include "functions.h"


void DanbooruTest::initTestCase()
{
	setLogFile("test_log.log");
}

void DanbooruTest::myInit(QString source)
{
	m_settings = new QSettings("test_settings.ini", QSettings::IniFormat);
	QSettings settings("../release/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", source);

	QList<Site*> sites;
	sites.append(new Site(m_settings, "../release/sites/Danbooru (2.0)", "danbooru.donmai.us"));

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


void DanbooruTest::testHtml()
{
    myInit("regex");

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
    QStringList expected = QStringList() << "testf2b78f4b9c79e6bef0cdc948fbd495cc" << "a6ce80f18652847857a0258f0046282d" << "91d66f4801086eb474fb5c52af8b0418";
    QCOMPARE(images.count(), 20);
    QCOMPARE(md5s, expected);
}

void DanbooruTest::testXml()
{
    myInit("xml");

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
