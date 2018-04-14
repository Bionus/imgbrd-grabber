#include "gelbooru-test.h"
#include <QStringList>
#include <QtTest>
#include "functions.h"


void GelbooruTest::testHtml()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Gelbooru (0.2)", "gelbooru.com", "regex", "rating:safe", "results.html", i != 0);

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (Image *img : images)
		{
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "2fd892ef1143793644e0b8f38e1c8849" << "b29ccbb2fcbeddad7a95b93c822ecbc0" << "31bf5040bafd4ebda1ae241857476b65";
		QCOMPARE(images.count(), 20);
		QCOMPARE(md5s, expected);
	}
}

void GelbooruTest::testXml()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Gelbooru (0.2)", "gelbooru.com", "xml", "rating:safe", "results.xml", i != 0);

		// Convert results
		QStringList md5s;
		md5s.reserve(images.count());
		for (Image *img : images)
		{
			md5s.append(img->md5());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "e70f631ce2bc1ab02b371489d81dceaa" << "ea2d6cd3dbe115401fc23b4ae3e7e7ab" << "67854632115557de382f26669228cec6";
		QCOMPARE(images.count(), 20);
		QCOMPARE(md5s, expected);
	}
}

void GelbooruTest::testPageTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getPageTags("Gelbooru (0.2)", "gelbooru.com", "regex", "rating:safe", "results.html", i != 0);

		QCOMPARE(tags.count(), 53);

		QCOMPARE(tags[0].text(), QString("00s"));
		QCOMPARE(tags[0].count(), 255610);
		QCOMPARE(tags[1].text(), QString("1girl"));
		QCOMPARE(tags[1].count(), 2302988);
		QCOMPARE(tags[2].text(), QString("aqua_hair"));
		QCOMPARE(tags[2].count(), 60142);
	}
}

void GelbooruTest::testHtmlTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getTags("Gelbooru (0.2)", "gelbooru.com", "regex", "tags.html", i != 0);

		QCOMPARE(tags.count(), 50);

		QCOMPARE(tags[3].text(), QString("nami_(one_piece)"));
		QCOMPARE(tags[3].count(), 5594);
		QCOMPARE(tags[3].type().name(), QString("character"));
	}
}


static GelbooruTest instance;
