#include "zerochan-test.h"
#include <QStringList>
#include <QtTest>
#include "models/image.h"
#include "tags/tag.h"


void ZerochanTest::testHtml()
{
	QList<Image*> images = getImages("Zerochan", "www.zerochan.net", "regex", "Touhou", "results.html");

	// Convert results
	QList<qulonglong> ids;
	ids.reserve(images.count());
	for (Image *img : images) {
		ids.append(img->id());
	}

	// Check results
	ids = ids.mid(0, 3);
	QList<qulonglong> expected = QList<qulonglong>() << 2034435 << 2034432 << 2034431;
	QCOMPARE(images.count(), 20);
	QCOMPARE(ids, expected);
}

void ZerochanTest::testRss()
{
	QList<Image*> images = getImages("Zerochan", "www.zerochan.net", "rss", "Touhou", "results.rss");

	// Convert results
	QList<qulonglong> ids;
	ids.reserve(images.count());
	for (Image *img : images) {
		ids.append(img->id());
	}

	// Check results
	ids = ids.mid(0, 3);
	QList<qulonglong> expected = QList<qulonglong>() << 2034435 << 2034432 << 2034431;
	QCOMPARE(images.count(), 20);
	QCOMPARE(ids, expected);
}


QTEST_MAIN(ZerochanTest)
