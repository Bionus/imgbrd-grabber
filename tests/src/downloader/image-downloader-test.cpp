#include "image-downloader-test.h"
#include <QtTest>
#include "custom-network-access-manager.h"
#include "models/image.h"
#include "models/site.h"
#include "models/source.h"


void ImageDownloaderTest::initTestCase()
{
	QDir("tests/resources/").mkdir("tmp");
}

void ImageDownloaderTest::cleanup()
{
	QDir dir("tests/resources/tmp/");
	for (const QString &file : dir.entryList(QDir::Files))
	{
		dir.remove(file);
	}

	delete m_profile;
	m_source->deleteLater();
	m_site->deleteLater();
}

Image *ImageDownloaderTest::createImage(bool noMd5)
{
	QMap<QString, QString> details;
	if (!noMd5)
	{ details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718"; }
	details["ext"] = "jpg";
	details["id"] = "7331";
	details["file_url"] = "http://test.com/img/oldfilename.jpg";
	details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	details["page_url"] = "/posts/7331";

	m_profile = new Profile("tests/resources/");
	m_source = new Source(m_profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
	return new Image(m_site, details, m_profile);
}


void ImageDownloaderTest::testSuccessBasic()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::SaveResult::Saved);

	assertDownload(img, &downloader, expected, true);
}

void ImageDownloaderTest::testSuccessLoadTags()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "%copyright%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/to heart 2.jpg"), Image::SaveResult::Saved);

	assertDownload(img, &downloader, expected, true);
}

void ImageDownloaderTest::testOpenError()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "///", "///root/toto", 1, false, false, nullptr, false, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("//root/toto/"), Image::SaveResult::Error);

	assertDownload(img, &downloader, expected, false, true);
}

void ImageDownloaderTest::testNotFound()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::SaveResult::NotFound);

	CustomNetworkAccessManager::NextFiles.append("404");

	assertDownload(img, &downloader, expected, false);
}

void ImageDownloaderTest::testNetworkError()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::SaveResult::NetworkError);

	CustomNetworkAccessManager::NextFiles.append("500");

	assertDownload(img, &downloader, expected, false);
}

void ImageDownloaderTest::testOriginalMd5()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/1bc29b36f623ba82aaf6724fd3b16718.jpg"), Image::SaveResult::Saved);

	assertDownload(img, &downloader, expected, true);
}

void ImageDownloaderTest::testGeneratedMd5()
{
	QSharedPointer<Image> img(createImage(true));
	ImageDownloader downloader(img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, false);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/956ddde86fb5ce85218b21e2f49e5c50.jpg"), Image::SaveResult::Saved);

	assertDownload(img, &downloader, expected, true);
}

void ImageDownloaderTest::testRotateExtension()
{
	QSharedPointer<Image> img(createImage());
	ImageDownloader downloader(img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, true);

	QMap<QString, Image::SaveResult> expected;
	expected.insert(QDir::toNativeSeparators("tests/resources/tmp/1bc29b36f623ba82aaf6724fd3b16718.png"), Image::SaveResult::Saved);

	CustomNetworkAccessManager::NextFiles.append("404");

	assertDownload(img, &downloader, expected, true);
}


void ImageDownloaderTest::assertDownload(QSharedPointer<Image> img, ImageDownloader *downloader, const QMap<QString, Image::SaveResult> &expected, bool shouldExist, bool onlyCheckValues)
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
	if (onlyCheckValues)
	{ QCOMPARE(result.values(), expected.values()); }
	else
	{ QCOMPARE(result, expected); }

	for (const QString &path : result.keys())
	{
		QFile f(path);
		bool exists = f.exists();
		QVERIFY(exists == shouldExist);
		if (exists)
		{ f.remove(); }
	}
}


QTEST_MAIN(ImageDownloaderTest)
