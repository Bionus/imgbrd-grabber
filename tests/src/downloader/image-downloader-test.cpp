#include <QDir>
#include <QSettings>
#include <QSignalSpy>
#include "custom-network-access-manager.h"
#include "downloader/image-downloader.h"
#include "models/filtering/blacklist.h"
#include "models/image.h"
#include "models/image-factory.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"
#include "source-helpers.h"


QSharedPointer<Image> createImage(Profile *profile, Site *site, bool noMd5 = false)
{
	QMap<QString, QString> details;
	if (!noMd5) {
		details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	}
	details["ext"] = "jpg";
	details["id"] = "7331";
	details["file_url"] = "http://test.com/img/oldfilename.jpg";
	details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	details["page_url"] = "/posts/7331";
	details["tags"] = "tag1 tag2 tag3";

	return QSharedPointer<Image>(ImageFactory::build(site, details, profile));
}

void assertDownload(Profile *profile, QSharedPointer<Image> img, ImageDownloader *downloader, const QList<ImageSaveResult> &expected, bool shouldExist, bool onlyCheckValues = false, bool sampleFallback = false, bool remove = true)
{
	const bool oldSampleFallback = profile->getSettings()->value("Save/samplefallback", true).toBool();
	profile->getSettings()->setValue("Save/samplefallback", sampleFallback);

	qRegisterMetaType<QList<ImageSaveResult>>();
	QSignalSpy spy(downloader, SIGNAL(saved(QSharedPointer<Image>, QList<ImageSaveResult>)));
	QTimer::singleShot(1, downloader, SLOT(save()));
	REQUIRE(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	auto out = arguments[0].value<QSharedPointer<Image>>();
	auto result = arguments[1].value<QList<ImageSaveResult>>();

	profile->getSettings()->setValue("Save/samplefallback", oldSampleFallback);

	REQUIRE(out == img);
	REQUIRE(result.count() == expected.count());
	for (int i = 0; i < result.count(); ++i) {
		if (!onlyCheckValues) {
			REQUIRE(result[i].path == expected[i].path);
		}
		REQUIRE(result[i].size == expected[i].size);
		REQUIRE(result[i].result == expected[i].result);
	}

	for (const ImageSaveResult &res : result) {
		QFile f(res.path);
		bool exists = f.exists();
		REQUIRE(exists == shouldExist);
		if (exists && remove) {
			f.remove();
		}
	}
}


TEST_CASE("ImageDownloader")
{
	QDir("tests/resources/").mkdir("tmp");

	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	QDir dir("tests/resources/tmp/");
	for (const QString &file : dir.entryList(QDir::Files)) {
		dir.remove(file);
	}

	SECTION("SuccessBasic")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true);
	}

	SECTION("SuccessLoadTags")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "%copyright%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/to heart 2.jpg"), Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true);
	}

	SECTION("SuccessLoadTagsExternal")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);

		// Delete already existing
		QFile logFile("tests/resources/tmp/savelog.txt");
		if (logFile.exists()) {
			logFile.remove();
		}

		QSettings *settings = profile->getSettings();
		settings->setValue("LogFiles/0/locationType", 1);
		settings->setValue("LogFiles/0/uniquePath", logFile.fileName());
		settings->setValue("LogFiles/0/content", "%copyright%");

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true);

		REQUIRE(logFile.exists());
		REQUIRE(logFile.open(QFile::ReadOnly | QFile::Text));
		REQUIRE(QString(logFile.readAll()) == QString("to heart 2"));

		logFile.close();
		logFile.remove();

		settings->remove("LogFiles/0/locationType");
		settings->remove("LogFiles/0/uniquePath");
		settings->remove("LogFiles/0/content");
	}

	SECTION("SuccessLoadSize")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "%copyright%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/to heart 2.jpg"), Image::Size::Full, Image::SaveResult::Saved });

		REQUIRE(img->size().isEmpty());
		assertDownload(profile, img, &downloader, expected, true);
		REQUIRE(img->size() == QSize(1, 1));
	}

	SECTION("OpenError")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "///", "///root/toto", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("//root/toto/"), Image::Size::Full, Image::SaveResult::Error });

		assertDownload(profile, img, &downloader, expected, false, true);
	}

	SECTION("NotFound")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::Size::Full, Image::SaveResult::NotFound });

		CustomNetworkAccessManager::NextFiles.append("404");

		assertDownload(profile, img, &downloader, expected, false);
	}

	SECTION("NetworkError")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::Size::Full, Image::SaveResult::NetworkError });

		CustomNetworkAccessManager::NextFiles.append("500");

		assertDownload(profile, img, &downloader, expected, false);
	}

	SECTION("OriginalMd5")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/1bc29b36f623ba82aaf6724fd3b16718.jpg"), Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true);
	}

	SECTION("GeneratedMd5")
	{
		auto img = createImage(profile, site, true);
		ImageDownloader downloader(profile, img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/956ddde86fb5ce85218b21e2f49e5c50.jpg"), Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true);
	}

	SECTION("RotateExtension")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, true);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/1bc29b36f623ba82aaf6724fd3b16718.png"), Image::Size::Full, Image::SaveResult::Saved });

		CustomNetworkAccessManager::NextFiles.append("404");

		assertDownload(profile, img, &downloader, expected, true);
	}

	SECTION("SampleFallback")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/1bc29b36f623ba82aaf6724fd3b16718.jpg"), Image::Size::Sample, Image::SaveResult::Saved });

		CustomNetworkAccessManager::NextFiles.append("404");

		assertDownload(profile, img, &downloader, expected, true, false, true);
	}

	SECTION("Blacklisted")
	{
		Blacklist blacklist(QStringList() << "tag1");

		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);
		downloader.setBlacklist(&blacklist);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/out.jpg"), Image::Size::Full, Image::SaveResult::Blacklisted });

		assertDownload(profile, img, &downloader, expected, false);

		profile->removeBlacklistedTag("tag1");
	}

	SECTION("Resize too big image")
	{
		QSettings *settings = profile->getSettings();
		settings->setValue("ImageSize/maxWidthEnabled", true);
		settings->setValue("ImageSize/maxWidth", 50);

		CustomNetworkAccessManager::NextFiles.enqueue("gui/resources/images/colors/original.jpg"); // 256x256
		QString path = QDir::toNativeSeparators("tests/resources/tmp/out.jpg");

		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ path, Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true, false, false, false);
		REQUIRE(QImage(path).size() == QSize(50, 50));
		QFile::remove(path);

		settings->remove("ImageSize/maxWidthEnabled");
		settings->remove("ImageSize/maxWidth");
	}

	SECTION("Resize already small image")
	{
		QSettings *settings = profile->getSettings();
		settings->setValue("ImageSize/maxWidthEnabled", true);
		settings->setValue("ImageSize/maxWidth", 500);

		CustomNetworkAccessManager::NextFiles.enqueue("gui/resources/images/colors/original.jpg"); // 256x256
		QString path = QDir::toNativeSeparators("tests/resources/tmp/out.jpg");

		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ path, Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true, false, false, false);
		REQUIRE(QImage(path).size() == QSize(256, 256));
		QFile::remove(path);

		settings->remove("ImageSize/maxWidthEnabled");
		settings->remove("ImageSize/maxWidth");
	}
}
