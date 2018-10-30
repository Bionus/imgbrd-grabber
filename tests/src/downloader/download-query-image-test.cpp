#include "download-query-image-test.h"
#include <QtTest>
#include "downloader/download-query-image.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


void DownloadQueryImageTest::testCompare()
{
	Profile profile("tests/resources/");
	Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	DownloadQueryImage a(1, "md5", "rating", "tags", "fileUrl", "date", &site, "filename", "path", QStringList() << "search");
	DownloadQueryImage b(1, "md5", "rating", "tags", "fileUrl", "date", &site, "filename", "path", QStringList() << "search");
	DownloadQueryImage c(2, "md5", "rating", "tags", "fileUrl", "date", &site, "filename", "path", QStringList() << "search");

	QVERIFY(a == b);
	QVERIFY(b == a);
	QVERIFY(a != c);
	QVERIFY(b != c);
	QVERIFY(c == c);
}

void DownloadQueryImageTest::testSerialization()
{
	Profile profile("tests/resources/");
	Source source(&profile, "tests/resources/sites/Danbooru (2.0)");
	Site site("danbooru.donmai.us", &source);

	DownloadQueryImage original(1, "md5", "rating", "tags", "fileUrl", "date", &site, "filename", "path", QStringList() << "search");

	QJsonObject json;
	original.write(json);

	DownloadQueryImage dest;
	dest.read(json, QMap<QString, Site*> {{ site.url(), &site }});

	QCOMPARE(dest.values["id"], QString("1"));
	QCOMPARE(dest.values["md5"], QString("md5"));
	QCOMPARE(dest.values["rating"], QString("rating"));
	QCOMPARE(dest.values["tags"], QString("tags"));
	QCOMPARE(dest.values["file_url"], QString("fileUrl"));
	QCOMPARE(dest.values["date"], QString("date"));
	QCOMPARE(dest.values["search"], QString("search"));
	QCOMPARE(dest.site, &site);
	QCOMPARE(dest.filename, QString("filename"));
	QCOMPARE(dest.path, QString("path"));
}


QTEST_MAIN(DownloadQueryImageTest)
