#include <QtTest>
#include <QStringList>
#include "sankaku-test.h"
#include "functions.h"


void SankakuTest::testHtml()
{
	QList<Image*> images = getImages("Sankaku", "idol.sankakucomplex.com", "regex", "rating:safe");

	// Convert results
	QStringList md5s;
	for (Image *img : images)
	{
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "7af162c8a2e5299d737de002fce087cf" << "8dd5c24458feb851c4dfbb302ebf5c06" << "33347fcbeb76b6d7d2c31a5d491d53ee";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void SankakuTest::testAnimatedUrls()
{
	QList<Image*> images = getImages("Sankaku", "idol.sankakucomplex.com", "regex", "animated rating:safe");

	// Convert results
	QStringList md5s, urls;
	for (Image *img : images)
	{
		md5s.append(img->md5());
		urls.append(img->url());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "6e7901eea2a5a2d2b96244593ed190df" << "97b3355a7af0bfabc67f2678a4a837fd" << "d9f7f5089da4a677846d77da2c146088";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);

	// Compare URLs
	QStringList expectedUrls = QStringList()
							   << "https://is.sankakucomplex.com/data/6e/79/6e7901eea2a5a2d2b96244593ed190df.gif"
							   << "https://is.sankakucomplex.com/data/97/b3/97b3355a7af0bfabc67f2678a4a837fd.gif"
							   << "https://is.sankakucomplex.com/data/d9/f7/d9f7f5089da4a677846d77da2c146088.webm";
	QStringList actualUrls = urls.mid(0, 3);
	QCOMPARE(actualUrls, expectedUrls);
}


static SankakuTest instance;
