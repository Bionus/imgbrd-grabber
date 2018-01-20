#include "sankaku-test.h"
#include <QStringList>
#include <QtTest>
#include "functions.h"


void SankakuTest::testHtml()
{
	QList<Image*> images = getImages("Sankaku", "idol.sankakucomplex.com", "regex", "rating:safe", "results.html");

	// Check results
	QCOMPARE(images.count(), 20);
	QCOMPARE(images[0]->md5(), QString("7af162c8a2e5299d737de002fce087cf"));
	QCOMPARE(images[1]->md5(), QString("8dd5c24458feb851c4dfbb302ebf5c06"));
	QCOMPARE(images[2]->md5(), QString("33347fcbeb76b6d7d2c31a5d491d53ee"));
}

void SankakuTest::testJson()
{
	QList<Image*> images = getImages("Sankaku", "idol.sankakucomplex.com", "json", "rating:safe", "results.json");

	// Check results
	QCOMPARE(images.count(), 20);
	QCOMPARE(images[0]->md5(), QString("26d8d649afde8fab74f1cf09607daebb"));
	QCOMPARE(images[0]->createdAt(), QDateTime::fromMSecsSinceEpoch(1484391423000));
	QCOMPARE(images[1]->md5(), QString("c68c77540ab3813c9bc7c5059f3a0ac2"));
	QCOMPARE(images[1]->createdAt(), QDateTime::fromMSecsSinceEpoch(1484391415000));
	QCOMPARE(images[2]->md5(), QString("6b154030d5b017b75917d160fc22203a"));
	QCOMPARE(images[2]->createdAt(), QDateTime::fromMSecsSinceEpoch(1484391403000));
}

void SankakuTest::testAnimatedUrls()
{
	QList<Image*> images = getImages("Sankaku", "idol.sankakucomplex.com", "regex", "animated rating:safe", "results-animated.html");

	// Check results
	QCOMPARE(images.count(), 20);
	QCOMPARE(images[0]->md5(), QString("6e7901eea2a5a2d2b96244593ed190df"));
	QCOMPARE(images[0]->url(), QString("https://is.sankakucomplex.com/data/6e/79/6e7901eea2a5a2d2b96244593ed190df.gif"));
	QCOMPARE(images[1]->md5(), QString("97b3355a7af0bfabc67f2678a4a837fd"));
	QCOMPARE(images[1]->url(), QString("https://is.sankakucomplex.com/data/97/b3/97b3355a7af0bfabc67f2678a4a837fd.gif"));
	QCOMPARE(images[2]->md5(), QString("d9f7f5089da4a677846d77da2c146088"));
	QCOMPARE(images[2]->url(), QString("https://is.sankakucomplex.com/data/d9/f7/d9f7f5089da4a677846d77da2c146088.webm"));
}


static SankakuTest instance;
