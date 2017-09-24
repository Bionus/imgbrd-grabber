#include <QtTest>
#include <QStringList>
#include "e621-test.h"
#include "functions.h"


void E621Test::testSwfUrls()
{
	QList<Image*> images = getImages("Danbooru", "e621.net", "regex", "swf rating:safe", "results.html");

	// Convert results
	QStringList md5s, urls;
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

void E621Test::testXmlTypedTags()
{
	QList<Image*> images = getImages("Danbooru", "e621.net", "xml", "rating:safe", "results-typed.xml");
	QVERIFY(!images.isEmpty());

	QList<Tag> tags = images.first()->tags();
	QCOMPARE(tags.count(), 22);

	QCOMPARE(tags[0].text(), QString("female"));
	QCOMPARE(tags[0].type().name(), QString("general"));
	QCOMPARE(tags[21].text(), QString("mammal"));
	QCOMPARE(tags[21].type().name(), QString("species"));
}

void E621Test::testJsonTypedTags()
{
	QList<Image*> images = getImages("Danbooru", "e621.net", "json", "rating:safe", "results-typed.json");
	QVERIFY(!images.isEmpty());

	QList<Tag> tags = images.first()->tags();
	QCOMPARE(tags.count(), 22);

	QCOMPARE(tags[0].text(), QString("lumineko"));
	QCOMPARE(tags[0].type().name(), QString("artist"));
	QCOMPARE(tags[21].text(), QString("equine"));
	QCOMPARE(tags[21].type().name(), QString("species"));
}

void E621Test::testHtmlTags()
{
	QList<Tag> tags = getTags("Danbooru", "e621.net", "regex", "tags.html");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[0].text(), QString("mammal"));
	QCOMPARE(tags[0].count(), 907884);
	QCOMPARE(tags[0].type().name(), QString("species"));
}

void E621Test::testXmlTags()
{
	QList<Tag> tags = getTags("Danbooru", "e621.net", "xml", "tags.xml");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[0].text(), QString("mammal"));
	QCOMPARE(tags[0].count(), 866534);
	QCOMPARE(tags[0].type().name(), QString("species"));
}

void E621Test::testJsonTags()
{
	QList<Tag> tags = getTags("Danbooru", "e621.net", "json", "tags.json");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[0].text(), QString("mammal"));
	QCOMPARE(tags[0].count(), 866534);
	QCOMPARE(tags[0].type().name(), QString("species"));
}


static E621Test instance;
