#include "image-downloader-test.h"
#include <QtTest>
#include "models/image.h"
#include "models/site.h"
#include "models/source.h"


Image *ImageDownloaderTest::createImage()
{
	QMap<QString, QString> details;
	details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	details["ext"] = "jpg";
	details["id"] = "7331";
	details["file_url"] = "http://test.com/img/oldfilename.jpg";
	details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "http://test.com/preview/oldfilename.jpg";

	auto *source = new Source(&profile, "release/sites/Danbooru (2.0)");
	auto *site = new Site("danbooru.donmai.us", source);
	return new Image(site, details, &profile);
}


void ImageDownloaderTest::testSuccessBasic()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "out.jpg", "tests/resources/tmp", 1, false, false, Q_NULLPTR, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::SaveResult::Saved);

	assertDownload(img, &downloader, expected, true);
}

void ImageDownloaderTest::testSuccessLoadTags()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "%copyright%.%ext%", "tests/resources/tmp", 1, false, false, Q_NULLPTR, true);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/to heart 2.jpg"), Image::SaveResult::Saved);

	assertDownload(img, &downloader, expected, true);
}

void ImageDownloaderTest::testOpenError()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "///", "///root/toto", 1, false, false, Q_NULLPTR, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("//root/toto/"), Image::SaveResult::Error);

	assertDownload(img, &downloader, expected, false);
}


void ImageDownloaderTest::assertDownload(QSharedPointer<Image> img, ImageDownloader *downloader, const QMap<QString, Image::SaveResult> &expected, bool shouldExist)
{
	qRegisterMetaType<QMap<QString, Image::SaveResult>>();
	QSignalSpy spy(downloader, SIGNAL(saved(QSharedPointer<Image>, QMap<QString, Image::SaveResult>)));
	QTimer::singleShot(1, downloader, SLOT(save()));
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	auto out = arguments[0].value<QSharedPointer<Image>>();
	auto result = arguments[1].value<QMap<QString, Image::SaveResult>>();

	QCOMPARE(out, img);
	qDebug() << "result" << result;
	qDebug() << "expected" << expected;
	QCOMPARE(result, expected);

	for (const QString &path : result.keys())
	{
		QFile f(path);
		bool exists = f.exists();
		QVERIFY(exists == shouldExist);
		if (exists)
		{ f.remove(); }
	}
}


static ImageDownloaderTest instance;
