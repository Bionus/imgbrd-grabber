#include "e621-test.h"
#include <QStringList>
#include <QtTest>
#include "functions.h"


void E621Test::testSwfUrls()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Danbooru", "e621.net", "regex", "swf rating:safe", "results.html", i != 0);

		// Convert results
		QStringList md5s, urls;
		md5s.reserve(images.count());
		urls.reserve(images.count());
		for (Image *img : images)
		{
			md5s.append(img->md5());
			urls.append(img->url());
		}

		// Check results
		md5s = md5s.mid(0, 3);
		QStringList expected = QStringList() << "cb0523430ab3a75216fe1b3a3a42cac5" << "4533e0a1bf7b132038f7ab3864ecd027" << "d8461800f2a107f2d928fcbca00f6019";
		QCOMPARE(images.count(), 20);
		QCOMPARE(md5s, expected);

		// Compare URLs
		QStringList expectedUrls = QStringList()
								   << "https://static1.e621.net/data/cb/05/cb0523430ab3a75216fe1b3a3a42cac5.swf"
								   << "https://static1.e621.net/data/45/33/4533e0a1bf7b132038f7ab3864ecd027.swf"
								   << "https://static1.e621.net/data/d8/46/d8461800f2a107f2d928fcbca00f6019.swf";
		QStringList actualUrls = urls.mid(0, 3);
		QCOMPARE(actualUrls, expectedUrls);
	}
}

void E621Test::testXmlTypedTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Danbooru", "e621.net", "xml", "rating:safe", "results-typed.xml", i != 0);
		QVERIFY(!images.isEmpty());

		QList<Tag> tags = images.first()->tags();
		QCOMPARE(tags.count(), 22);

		QCOMPARE(tags[0].text(), QString("female"));
		QCOMPARE(tags[0].type().name(), QString("general"));
		QCOMPARE(tags[21].text(), QString("mammal"));
		QCOMPARE(tags[21].type().name(), QString("species"));
	}
}

void E621Test::testJsonTypedTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Image*> images = getImages("Danbooru", "e621.net", "json", "rating:safe", "results-typed.json", i != 0);
		QVERIFY(!images.isEmpty());

		QList<Tag> tags = images.first()->tags();
		QCOMPARE(tags.count(), 22);

		QCOMPARE(tags[21].text(), QString("equine"));
		QCOMPARE(tags[21].type().name(), QString("species"));
	}
}

void E621Test::testHtmlTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getTags("Danbooru", "e621.net", "regex", "tags.html", i != 0);

		QCOMPARE(tags.count(), 100);

		QCOMPARE(tags[0].text(), QString("mammal"));
		QCOMPARE(tags[0].count(), 907884);
		QCOMPARE(tags[0].type().name(), QString("species"));
	}
}

void E621Test::testXmlTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getTags("Danbooru", "e621.net", "xml", "tags.xml", i != 0);

		QCOMPARE(tags.count(), 100);

		QCOMPARE(tags[0].text(), QString("mammal"));
		QCOMPARE(tags[0].count(), 866534);
		QCOMPARE(tags[0].type().name(), QString("species"));
	}
}

void E621Test::testJsonTags()
{
	for (int i = 0; i < 2; ++i)
	{
		QList<Tag> tags = getTags("Danbooru", "e621.net", "json", "tags.json", i != 0);

		QCOMPARE(tags.count(), 100);

		QCOMPARE(tags[0].text(), QString("mammal"));
		QCOMPARE(tags[0].count(), 866534);
		QCOMPARE(tags[0].type().name(), QString("species"));
	}
}


static E621Test instance;
