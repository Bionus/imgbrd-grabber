#include "danbooru-test.h"
#include <QStringList>
#include <QtTest>
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
	QStringList expected = QStringList() << "12a54c9a24868a6c717759f1dfef5864" << "b46086a869da3443181f7798c6918058" << "9aeff7f9ffddb7c6db36133be4ad4ca3";
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

	QCOMPARE(tags[0].text(), QString("solo"));
	QCOMPARE(tags[0].count(), 1805000);
	QCOMPARE(tags[1].text(), QString("looking_at_viewer"));
	QCOMPARE(tags[1].count(), 707000);
	QCOMPARE(tags[2].text(), QString("1girl"));
	QCOMPARE(tags[2].count(), 2177000);
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


QTEST_MAIN(DanbooruTest)
