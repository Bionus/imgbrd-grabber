#include <QtTest>
#include <QStringList>
#include "danbooru-test.h"
#include "functions.h"


void DanbooruTest::testHtml()
{
	QList<Image*> images = getImages("Danbooru (2.0)", "danbooru.donmai.us", "regex", "rating:safe", "results.html");

	// Convert results
	QStringList md5s;
	md5s.reserve(images.count());
	for (Image *img : images)
	{
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "b9e420f7d473b4c1b70b97304760d980" << "0eff70b9a01c59e134a1b11e763647da" << "57595a142730c917e534863c4033d06e";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void DanbooruTest::testXml()
{
	QList<Image*> images = getImages("Danbooru (2.0)", "danbooru.donmai.us", "xml", "rating:safe", "results.xml");

	// Convert results
	QStringList md5s;
	md5s.reserve(images.count());
	for (Image *img : images)
	{
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "test98bf686ca7910ee0ad48e59ba99807d3" << "e5d074c5fe05e1493372e7224d2d198f" << "1e774a4a9d080611fde61e58625e038e";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void DanbooruTest::testPageTags()
{
	QList<Tag> tags = getPageTags("Danbooru (2.0)", "danbooru.donmai.us", "regex", "rating:safe", "results.html");

	QCOMPARE(tags.count(), 25);

	QCOMPARE(tags[0].text(), QString("1girl"));
	QCOMPARE(tags[0].count(), 1964000);
	QCOMPARE(tags[1].text(), QString("solo"));
	QCOMPARE(tags[1].count(), 1635000);
	QCOMPARE(tags[2].text(), QString("long_hair"));
	QCOMPARE(tags[2].count(), 1240000);
}

void DanbooruTest::testHtmlTags()
{
	QList<Tag> tags = getTags("Danbooru (2.0)", "danbooru.donmai.us", "regex", "tags.html");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[1].text(), QString("apollo_star"));
	QCOMPARE(tags[1].count(), 1);
	QCOMPARE(tags[1].type().name(), QString("artist"));
}

void DanbooruTest::testXmlTags()
{
	QList<Tag> tags = getTags("Danbooru (2.0)", "danbooru.donmai.us", "xml", "tags.xml");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[1].text(), QString("walkr"));
	QCOMPARE(tags[1].count(), 1);
	QCOMPARE(tags[1].type().name(), QString("copyright"));
}

void DanbooruTest::testJsonTags()
{
	QList<Tag> tags = getTags("Danbooru (2.0)", "danbooru.donmai.us", "json", "tags.json");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[1].text(), QString("walkr"));
	QCOMPARE(tags[1].count(), 1);
	QCOMPARE(tags[1].type().name(), QString("copyright"));
}


static DanbooruTest instance;
