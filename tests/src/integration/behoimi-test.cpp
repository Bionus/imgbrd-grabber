#include "behoimi-test.h"
#include <QStringList>
#include <QtTest>
#include "models/image.h"
#include "tags/tag.h"


void BehoimiTest::testHtml()
{
	QList<Image*> images = getImages("Danbooru", "behoimi.org", "regex", "blue_legwear rating:safe", "results.html");

	// Convert results
	QStringList md5s;
	md5s.reserve(images.count());
	for (Image *img : images) {
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "2bdf03f0d3e7c5dcdfadaedc0434093e" << "1073770a3b0b565e1d0593620f28c0d6" << "5a49bcb7e90322c1edf866900e61ba1f";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void BehoimiTest::testXml()
{
	QList<Image*> images = getImages("Danbooru", "behoimi.org", "xml", "rating:safe", "results.xml");

	// Convert results
	QStringList md5s;
	md5s.reserve(images.count());
	for (Image *img : images) {
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "129577287dc57940398169481e7423cb" << "5a4d01cfbecc2a293d46df70144d6441" << "af7b48d271422dfdeb24c6dd102a8d50";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void BehoimiTest::testJson()
{
	QList<Image*> images = getImages("Danbooru", "behoimi.org", "json", "rating:safe", "results.json");

	// Convert results
	QStringList md5s;
	md5s.reserve(images.count());
	for (Image *img : images) {
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "129577287dc57940398169481e7423cb" << "5a4d01cfbecc2a293d46df70144d6441" << "af7b48d271422dfdeb24c6dd102a8d50";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void BehoimiTest::testPageTags()
{
	QList<Tag> tags = getPageTags("Danbooru", "behoimi.org", "regex", "blue_legwear rating:safe", "results.html");

	QCOMPARE(tags.count(), 25);

	QCOMPARE(tags[0].text(), QString("blue_legwear"));
	QCOMPARE(tags[0].count(), 295);
	QCOMPARE(tags[1].text(), QString("cosplay"));
	QCOMPARE(tags[1].count(), 295);
	QCOMPARE(tags[2].text(), QString("thighhighs"));
	QCOMPARE(tags[2].count(), 222);
}

void BehoimiTest::testHtmlTags()
{
	QList<Tag> tags = getTags("Danbooru", "behoimi.org", "regex", "tags.html");

	QCOMPARE(tags.count(), 50);

	QCOMPARE(tags[2].text(), QString("104"));
	QCOMPARE(tags[2].count(), 9);
	QCOMPARE(tags[2].type().name(), QString("model"));
}

void BehoimiTest::testXmlTags()
{
	QList<Tag> tags = getTags("Danbooru", "behoimi.org", "xml", "tags.xml");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[3].text(), QString("okubo_mariko"));
	QCOMPARE(tags[3].count(), 286);
	QCOMPARE(tags[3].type().name(), QString("model"));
}

void BehoimiTest::testJsonTags()
{
	QList<Tag> tags = getTags("Danbooru", "behoimi.org", "json", "tags.json");

	QCOMPARE(tags.count(), 100);

	QCOMPARE(tags[1].text(), QString("07_ghost"));
	QCOMPARE(tags[1].count(), 3);
	QCOMPARE(tags[1].type().name(), QString("copyright"));
}


QTEST_MAIN(BehoimiTest)
