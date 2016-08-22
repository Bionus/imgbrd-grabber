#include <QtTest>
#include <QStringList>
#include "sankaku-test.h"
#include "models/site.h"
#include "functions.h"


void SankakuTest::initTestCase()
{
	setLogFile("tests/test_log.log");
}

void SankakuTest::myInit(QString source)
{
	m_settings = new QSettings("tests/test_settings.ini", QSettings::IniFormat);
    QSettings settings("release/sites/Sankaku/idol.sankakucomplex.com/settings.ini", QSettings::IniFormat);
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", source);

	QList<Site*> sites;
    sites.append(new Site(m_settings, "release/sites/Sankaku", "idol.sankakucomplex.com"));

    m_downloader = new Downloader(QStringList() << "animated",
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

void SankakuTest::cleanup()
{
	m_downloader->deleteLater();
	m_settings->deleteLater();
}


void SankakuTest::testHtml()
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
    QStringList expected = QStringList() << "0c007c1cae6bb1327a87ea310b0ddf2a" << "3e58d450896af686e5022bbbb805a396" << "ca880454c31a5576ce701a3d58e165c4";
    QCOMPARE(images.count(), 20);
    QCOMPARE(md5s, expected);
}


static SankakuTest instance;
