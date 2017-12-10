#include <QtTest>
#include <QStringList>
#include "booru-org-test.h"
#include "functions.h"


void BooruOrgTest::testHtml()
{
	QList<Image*> images = getImages("Gelbooru (0.1)", "rm.booru.org", "regex", "rating:safe", "results.html");

	// Convert results
	QStringList md5s;
	md5s.reserve(images.count());
	for (Image *img : images)
	{
		md5s.append(img->md5());
	}

	// Check results
	md5s = md5s.mid(0, 3);
	QStringList expected = QStringList() << "88407041cfd2d8358dda2f8699bfe98d84a7cf74" << "e0c2ddaf9403901cc1e293bcd369806d1deffd95" << "44f0f9560431d1b61ba1e9c401fdb3cc75920b38";
	QCOMPARE(images.count(), 20);
	QCOMPARE(md5s, expected);
}

void BooruOrgTest::testPageTags()
{
	QList<Tag> tags = getPageTags("Gelbooru (0.1)", "rm.booru.org", "regex", "rating:safe", "results.html");

	QCOMPARE(tags.count(), 5);

	QCOMPARE(tags[0].text(), QString("barasuishou"));
	QCOMPARE(tags[0].count(), 4825);
	QCOMPARE(tags[1].text(), QString("image"));
	QCOMPARE(tags[1].count(), 94810);
	QCOMPARE(tags[2].text(), QString("rozen_maiden"));
	QCOMPARE(tags[2].count(), 125996);
}


static BooruOrgTest instance;
