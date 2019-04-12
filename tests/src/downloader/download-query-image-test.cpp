#include "download-query-image-test.h"
#include <QMap>
#include <QSharedPointer>
#include <QtTest>
#include "downloader/download-query-image.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"


void DownloadQueryImageTest::testCompare()
{
	Profile profile("tests/resources/");
	Site *site = profile.getSites().value("danbooru.donmai.us");

	auto img1 = QSharedPointer<Image>(new Image(site, {{ "id", "1" }}, &profile));
	auto img2 = QSharedPointer<Image>(new Image(site, {{ "id", "2" }}, &profile));

	DownloadQueryImage a(img1, site, "filename", "path");
	DownloadQueryImage b(img1, site, "filename", "path");
	DownloadQueryImage c(img2, site, "filename", "path");

	QVERIFY(a == b);
	QVERIFY(b == a);
	QVERIFY(a != c);
	QVERIFY(b != c);
	QVERIFY(c == c);
}

void DownloadQueryImageTest::testSerialization()
{
	Profile profile("tests/resources/");
	Site *site = profile.getSites().value("danbooru.donmai.us");

	QMap<QString, QString> details = {
		{ "id", "1" },
		{ "md5", "md5" },
		{ "rating", "rating" },
		{ "tags", "tags" },
		{ "file_url", "https://test.com/fileUrl" },
		{ "date", "2016-08-26T16:26:30+01:00" },
		{ "search", "search" },
	};
	auto img = QSharedPointer<Image>(new Image(site, details, &profile));
	DownloadQueryImage original(img, site, "filename", "path");

	QJsonObject json;
	original.write(json);

	DownloadQueryImage dest;
	dest.read(json, &profile);

	QCOMPARE(static_cast<int>(dest.image->id()), 1);
	QCOMPARE(dest.image->md5(), QString("md5"));
	QCOMPARE(dest.image->rating(), QString("rating"));
	QCOMPARE(dest.image->tagsString(), QStringList() << "tags");
	QCOMPARE(dest.image->fileUrl().toString(), QString("https://test.com/fileUrl"));
	QCOMPARE(dest.image->createdAt().toString("yyyy-MM-dd HH:mm:ss"), QString("2016-08-26 16:26:30"));
	QCOMPARE(dest.image->search(), QStringList() << "search");

	QCOMPARE(dest.site, site);
	QCOMPARE(dest.filename, QString("filename"));
	QCOMPARE(dest.path, QString("path"));
}


QTEST_MAIN(DownloadQueryImageTest)
