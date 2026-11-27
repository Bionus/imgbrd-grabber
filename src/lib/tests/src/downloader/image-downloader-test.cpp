#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QSet>
#include <QSettings>
#include <QSignalSpy>
#include "custom-network-access-manager.h"
#include "downloader/image-downloader.h"
#include "external/exiftool.h"
#include "functions.h"
#include "models/filtering/blacklist.h"
#include "models/image.h"
#include "models/image-factory.h"
#include "models/metadata-database/metadata-database.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"
#include "source-helpers.h"


QSharedPointer<Image> createImage(Profile *profile, Site *site, bool noMd5 = false, const QDateTime &createdAt = QDateTime())
{
	QMap<QString, QString> details;
	if (!noMd5) {
		details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	}
	details["ext"] = "jpg";
	details["id"] = "7331";
	details["file_url"] = "https://test.com/img/oldfilename.jpg";
	details["sample_url"] = "https://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "https://test.com/preview/oldfilename.jpg";
	details["page_url"] = "/posts/7331";
	details["tags"] = "tag1 tag2 tag3";
	if (createdAt.isValid()) {
		details["created_at"] = QString::number(createdAt.toSecsSinceEpoch());
	}

	return ImageFactory::build(site, details, profile);
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
	auto *profile = pProfile.data();
	profile->getSettings()->setValue("Save/headerDetection", false); // Necessary because the fake file is a PNG

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

		CustomNetworkAccessManager::NextFiles.enqueue("../gui/resources/images/colors/original.jpg"); // 256x256
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

		CustomNetworkAccessManager::NextFiles.enqueue("../gui/resources/images/colors/original.jpg"); // 256x256
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

	SECTION("Skip details for existing images")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "something.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/something.jpg.tmp"), Image::Size::Full, Image::SaveResult::AlreadyExistsMd5 });

		profile->getSettings()->setValue("Save/md5Duplicates", "ignore");
		profile->getSettings()->setValue("Save/md5DuplicatesSameDir", "save");
		profile->getSettings()->setValue("Exec/SQL/image", "SELECT %copyright%");
		profile->addMd5(img->md5(), "tests/resources/image_1x1.png");

		assertDownload(profile, img, &downloader, expected, false);
		REQUIRE(img->token("copyright", QString()) == QString());
	}

	SECTION("Skip details for existing images (same dir)")
	{
		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "something.%ext%", "tests/resources", 1, false, false, nullptr, true, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/something.jpg.tmp"), Image::Size::Full, Image::SaveResult::AlreadyExistsMd5 });

		profile->getSettings()->setValue("Save/md5Duplicates", "save");
		profile->getSettings()->setValue("Save/md5DuplicatesSameDir", "ignore");
		profile->getSettings()->setValue("Exec/SQL/image", "SELECT %copyright%");
		profile->addMd5(img->md5(), "tests/resources/image_1x1.png");

		assertDownload(profile, img, &downloader, expected, false);
		REQUIRE(img->token("copyright", QString()) == QString());
	}

	SECTION("Only hash existing destinations missing their source MD5 registration")
	{
		const bool registered = GENERATE(true, false);
		auto img = createImage(profile, site);
		const QString path = QDir::toNativeSeparators("tests/resources/tmp/out.jpg");
		REQUIRE(QFile("tests/resources/image_1x1.png").copy(path));
		if (registered) {
			profile->addMd5(img->md5(), path);
		}

		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, true, false, nullptr, false, false);
		const QList<ImageSaveResult> expected {{ path, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }};
		assertDownload(profile, img, &downloader, expected, true, false, false, false);

		const QStringList actualHashPaths = profile->md5Exists("956ddde86fb5ce85218b21e2f49e5c50");
		REQUIRE(actualHashPaths == (registered ? QStringList() : QStringList { path }));
		REQUIRE(QFile::remove(path));
	}

	SECTION("Do not rehash registered destinations when saving multiple paths")
	{
		auto img = createImage(profile, site);
		const QString existing = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
		const QString added = QDir::toNativeSeparators("tests/resources/tmp/added.jpg");
		REQUIRE(QFile("tests/resources/image_1x1.png").copy(existing));
		profile->addMd5(img->md5(), existing);
		profile->getSettings()->setValue("Save/md5Duplicates", "copy");
		profile->getSettings()->setValue("Save/md5DuplicatesSameDir", "copy");

		ImageDownloader downloader(profile, img, QStringList { existing, added }, 1, true, false, nullptr, false);
		const QList<ImageSaveResult> expected {
			{ existing, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk },
			{ added, Image::Size::Full, Image::SaveResult::Copied },
		};
		assertDownload(profile, img, &downloader, expected, true, false, false, false);

		REQUIRE(profile->md5Exists("956ddde86fb5ce85218b21e2f49e5c50").isEmpty());
		REQUIRE(profile->md5Exists(img->md5()).contains(added));
		REQUIRE(QFile::remove(existing));
		REQUIRE(QFile::remove(added));
	}

	SECTION("Skip details")
	{
		auto img = createImage(profile, site);
		QSignalSpy detailsSpy(img.data(), &Image::finishedLoadingTags);
		QSettings *settings = profile->getSettings();
		settings->setValue("Save/md5Duplicates", "copy");
		settings->setValue("Save/md5DuplicatesSameDir", "copy");

		SECTION("Copy to a new destination")
		{
			const bool sameDir = GENERATE(false, true);
			const QString tagRequirement = GENERATE(QString(), QString("Exec/SQL/image"), QString("LogFiles/0/content"));
			CAPTURE(sameDir, tagRequirement);
			const QString source = QDir::toNativeSeparators(sameDir ? "tests/resources/tmp/source.png" : "tests/resources/image_1x1.png");
			if (sameDir) {
				REQUIRE(QFile("tests/resources/image_1x1.png").copy(source));
			}
			settings->setValue(sameDir ? "Save/md5Duplicates" : "Save/md5DuplicatesSameDir", "save");
			if (!tagRequirement.isEmpty()) {
				settings->setValue(tagRequirement, "%copyright%");
			}
			if (tagRequirement == "LogFiles/0/content") {
				settings->setValue("LogFiles/0/locationType", 1);
				settings->setValue("LogFiles/0/uniquePath", "tests/resources/tmp/savelog.txt");
			}
			profile->addMd5(img->md5(), source);

			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/something.jpg");
			ImageDownloader downloader(profile, img, "something.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::Copied }}, true);
			REQUIRE(detailsSpy.count() == (tagRequirement.isEmpty() ? 0 : 1));
			REQUIRE(img->token("copyright", QString()) == (tagRequirement.isEmpty() ? QString() : QString("to_heart_2")));
			REQUIRE(QFile::exists(source));
			if (sameDir) {
				REQUIRE(QFile::remove(source));
			}
		}

		SECTION("Destination already exists")
		{
			const QString filename = GENERATE(QString("something.%ext%"), QString("%id%_%md5%.%ext%"));
			const bool registered = GENERATE(false, true);
			const bool metadata = GENERATE(false, true);
			CAPTURE(filename, registered, metadata);
			const QString dest = img->paths(filename, "tests/resources/tmp", 1).first();
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));
			settings->setValue("Exec/SQL/image", "SELECT %copyright%");
			if (registered) {
				profile->addMd5(img->md5(), dest);
			}
			if (metadata) {
				settings->beginWriteArray("Save/MetadataExiftool");
				settings->setArrayIndex(0);
				settings->setValue("key", "XMP:Creator");
				settings->setValue("value", "%copyright%");
				settings->endArray();
			}

			ImageDownloader downloader(profile, img, filename, "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true);
			REQUIRE(detailsSpy.isEmpty());
			REQUIRE(img->token("copyright", QString()) == QString());
		}

		SECTION("Filename needs details")
		{
			const bool sourceInTarget = GENERATE(false, true);
			const bool subdirectory = GENERATE(false, true);
			CAPTURE(sourceInTarget, subdirectory);
			const QString sourceDir = "tests/resources/tmp/artist";
			const QString source = QDir::toNativeSeparators(sourceInTarget ? sourceDir + "/already.png" : "tests/resources/image_1x1.png");
			if (sourceInTarget) {
				REQUIRE(QDir().mkpath(sourceDir));
				REQUIRE(QFile("tests/resources/image_1x1.png").copy(source));
			}
			profile->addMd5(img->md5(), source);

			const QString filename = subdirectory ? "%copyright%/%md5%.%ext%" : "%copyright%.%ext%";
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/" + (subdirectory ? "to heart 2/" + img->md5() : "to heart 2") + ".jpg");
			ImageDownloader downloader(profile, img, filename, "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::Copied }}, true);
			REQUIRE(detailsSpy.count() == 1);
			REQUIRE(img->token("copyright", QString()) == QString("to_heart_2"));
			REQUIRE(QFile::exists(source));
			if (sourceInTarget) {
				REQUIRE(QFile::remove(source));
				REQUIRE(QDir().rmdir(sourceDir));
			}
			if (subdirectory) {
				REQUIRE(QDir().rmdir("tests/resources/tmp/to heart 2"));
			}
		}

		SECTION("New copy needs details for metadata")
		{
			const QString oldName = QDir::toNativeSeparators("tests/resources/tmp/1_1bc29b36f623ba82aaf6724fd3b16718.jpg");
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/7331_1bc29b36f623ba82aaf6724fd3b16718.jpg");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(oldName));
			profile->addMd5(img->md5(), oldName);
			settings->beginWriteArray("Save/MetadataExiftool");
			settings->setArrayIndex(0);
			settings->setValue("key", "XMP:Creator");
			settings->setValue("value", "%copyright%");
			settings->endArray();

			ImageDownloader downloader(profile, img, "%id%_%md5%.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::Copied }}, true);
			REQUIRE(detailsSpy.count() == 1);
			REQUIRE(img->token("copyright", QString()) == QString("to_heart_2"));
			REQUIRE(QFile::remove(oldName));
			QFile::remove(dest + ".xmp");
		}
	}

	SECTION("Update metadata of existing files")
	{
		QSettings *settings = profile->getSettings();
		settings->setValue("Save/MetadataUpdateExisting", true);
		settings->setValue("Save/MetadataDatabase", true);
		settings->beginWriteArray("Save/MetadataExiftool");
		settings->setArrayIndex(0);
		settings->setValue("key", "XMP:Creator");
		settings->setValue("value", "%copyright%");
		settings->endArray();

		SECTION("Destination already exists")
		{
			auto img = createImage(profile, site);
			QSignalSpy detailsSpy(img.data(), &Image::finishedLoadingTags);
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));

			ImageDownloader downloader(profile, img, "existing.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true);
			REQUIRE(detailsSpy.count() == 1);
			REQUIRE(img->token("copyright", QString()) == QString("to_heart_2"));

			const QList<MetadataDatabaseEntry> entries = profile->metadataDatabase()->get(img->md5());
			REQUIRE(entries.count() == 1);
			REQUIRE(entries[0].website == QString("danbooru.donmai.us"));
			REQUIRE(entries[0].id == QString("7331"));
			REQUIRE(entries[0].tags.value("copyright") == QStringList("to_heart_2"));
			QFile::remove(dest + ".xmp");
		}

		SECTION("File already exists elsewhere and MD5 duplicates are ignored")
		{
			auto img = createImage(profile, site);
			QSignalSpy detailsSpy(img.data(), &Image::finishedLoadingTags);
			const QString duplicate = QDir::toNativeSeparators("tests/resources/tmp/duplicate.png");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(duplicate));
			profile->addMd5(img->md5(), duplicate);
			settings->setValue("Save/md5Duplicates", "ignore");
			settings->setValue("Save/md5DuplicatesSameDir", "ignore");

			ImageDownloader downloader(profile, img, "something.%ext%", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ QDir::toNativeSeparators("tests/resources/tmp/something.jpg.tmp"), Image::Size::Full, Image::SaveResult::AlreadyExistsMd5 }}, false);
			REQUIRE(detailsSpy.count() == 1);
			REQUIRE(QFile::exists(duplicate));
			REQUIRE(profile->metadataDatabase()->get(img->md5()).count() == 1);
			QFile::remove(duplicate + ".xmp");
		}

		SECTION("Only once per website")
		{
			const bool alreadyFetched = GENERATE(false, true);
			CAPTURE(alreadyFetched);
			settings->setValue("Save/MetadataUpdateOncePerSource", true);

			auto img = createImage(profile, site);
			QSignalSpy detailsSpy(img.data(), &Image::finishedLoadingTags);
			if (alreadyFetched) {
				REQUIRE(profile->metadataDatabase()->save(img->metadataDatabaseEntry()));
			}
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));

			ImageDownloader downloader(profile, img, "existing.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true);
			REQUIRE(detailsSpy.count() == (alreadyFetched ? 0 : 1));

			const QList<MetadataDatabaseEntry> entries = profile->metadataDatabase()->get(img->md5());
			REQUIRE(entries.count() == 1);
			REQUIRE(entries[0].tags.value("copyright") == (alreadyFetched ? QStringList() : QStringList("to_heart_2")));
			QFile::remove(dest + ".xmp");
		}

		SECTION("Maximum age without a post date uses the file date")
		{
			const bool recent = GENERATE(false, true);
			CAPTURE(recent);
			settings->setValue("Save/MetadataUpdateMaxAge", 7);
			settings->remove("Save/MetadataExiftool"); // Don't load details as they would set a post date

			auto img = createImage(profile, site);
			REQUIRE(!img->createdAt().isValid());
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));
			REQUIRE(setFileCreationDate(dest, QDateTime::currentDateTime().addDays(recent ? -2 : -30)));

			ImageDownloader downloader(profile, img, "existing.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true);
			REQUIRE(profile->metadataDatabase()->get(img->md5()).count() == (recent ? 1 : 0));
		}

		SECTION("Restores the post date of the file after updating it")
		{
			settings->setValue("Save/keepDate", true);
			settings->remove("Save/MetadataExiftool"); // Don't load details as they would change the post date

			const QDateTime createdAt = QDateTime::currentDateTime().addDays(-10);
			auto img = createImage(profile, site, false, createdAt);
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));

			ImageDownloader downloader(profile, img, "existing.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true, false, false, false);
			REQUIRE(QFileInfo(dest).lastModified().toSecsSinceEpoch() == createdAt.toSecsSinceEpoch());
			REQUIRE(QFile::remove(dest));
		}

		SECTION("Keeps the file date when the post has no date")
		{
			// Write a token that doesn't need details (which would give a post date), into a real PNG so that Exiftool rewrites it
			settings->beginWriteArray("Save/MetadataExiftool");
			settings->setArrayIndex(0);
			settings->setValue("key", "XMP:Creator");
			settings->setValue("value", "%id%");
			settings->endArray();

			auto img = createImage(profile, site);
			REQUIRE(!img->createdAt().isValid());
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.png");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));
			const QDateTime fileDate = QDateTime::currentDateTime().addDays(-30);
			REQUIRE(setFileCreationDate(dest, fileDate));

			ImageDownloader downloader(profile, img, "existing.png", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true, false, false, false);
			REQUIRE(QFileInfo(dest).lastModified().toSecsSinceEpoch() == fileDate.toSecsSinceEpoch());

			// Make sure the metadata was actually written into the file, which resets its date
			if (!Exiftool::version().isEmpty()) {
				QProcess process;
				process.start("exiftool", { "-s3", "-XMP:Creator", dest });
				REQUIRE(process.waitForFinished());
				REQUIRE(QString::fromUtf8(process.readAllStandardOutput()).trimmed() == QString("7331"));
			}

			REQUIRE(QFile::remove(dest));
		}

		SECTION("Writes the tags of all known sources to the XMP sidecar")
		{
			if (Exiftool::version().isEmpty()) {
				SKIP("Exiftool is not installed");
			}

			// Write all the merged tags as a list, only in the sidecar file since the test image is not a real JPEG
			// The copyright requires typed tags, so it forces loading the latest details of the post
			settings->beginWriteArray("Save/MetadataExiftool");
			settings->setArrayIndex(0);
			settings->setValue("key", "XMP:Subject");
			settings->setValue("value", "%merged_all:separator=;%");
			settings->setArrayIndex(1);
			settings->setValue("key", "XMP:Creator");
			settings->setValue("value", "%copyright%");
			settings->endArray();
			settings->setValue("Save/MetadataExiftoolSidecar", "only");

			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
			const QString sidecar = dest + ".xmp";
			const auto readSubjects = [&sidecar]() {
				QProcess process;
				process.start("exiftool", { "-s3", "-sep", ";", "-XMP:Subject", sidecar });
				REQUIRE(process.waitForFinished());
				QStringList subjects = QString::fromUtf8(process.readAllStandardOutput()).trimmed().split(';', Qt::SkipEmptyParts);
				for (QString &subject : subjects) {
					subject = subject.trimmed().replace('_', ' ');
				}
				return subjects;
			};
			const auto update = [&]() {
				auto img = createImage(profile, site);
				REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));
				ImageDownloader downloader(profile, img, "existing.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);
				assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true);
				return img;
			};

			// Another website already has data about this file
			MetadataDatabaseEntry gelbooru;
			gelbooru.md5 = "1bc29b36f623ba82aaf6724fd3b16718";
			gelbooru.website = "gelbooru.com";
			gelbooru.websiteName = "Gelbooru";
			gelbooru.id = "5680029";
			gelbooru.tags.insert("general", { "tag4" });
			gelbooru.allTags = QStringList { "tag4" };
			REQUIRE(profile->metadataDatabase()->save(gelbooru));

			// The sidecar contains the tags of both websites
			auto img = update();
			REQUIRE(QFile::exists(sidecar));
			QStringList subjects = readSubjects();
			CAPTURE(subjects);
			REQUIRE(subjects.contains("tag4"));
			REQUIRE(subjects.contains("to heart 2"));
			REQUIRE(!subjects.contains("tag1")); // Tags from the search results are replaced by the latest ones
			REQUIRE(profile->metadataDatabase()->get(img->md5()).count() == 2);

			// Updating again replaces the list instead of appending to it
			REQUIRE(profile->metadataDatabase()->remove(img->md5()));
			update();
			subjects = readSubjects();
			REQUIRE(!subjects.contains("tag4"));
			REQUIRE(subjects.contains("to heart 2"));
			REQUIRE(subjects.count() == QSet<QString>(subjects.begin(), subjects.end()).count());

			QFile::remove(sidecar);
		}

		SECTION("Maximum age")
		{
			const bool recent = GENERATE(false, true);
			CAPTURE(recent);
			settings->setValue("Save/MetadataUpdateMaxAge", 7);
			settings->remove("Save/MetadataExiftool"); // Don't load details as they would override the post date

			const QDateTime createdAt = recent ? QDateTime::currentDateTime().addDays(-2) : QDateTime::currentDateTime().addDays(-30);
			auto img = createImage(profile, site, false, createdAt);
			QSignalSpy detailsSpy(img.data(), &Image::finishedLoadingTags);
			const QString dest = QDir::toNativeSeparators("tests/resources/tmp/existing.jpg");
			REQUIRE(QFile("tests/resources/image_1x1.png").copy(dest));

			ImageDownloader downloader(profile, img, "existing.jpg", "tests/resources/tmp", 1, false, false, nullptr, true, false);
			assertDownload(profile, img, &downloader, {{ dest, Image::Size::Full, Image::SaveResult::AlreadyExistsDisk }}, true);
			REQUIRE(detailsSpy.isEmpty());
			REQUIRE(profile->metadataDatabase()->get(img->md5()).count() == (recent ? 1 : 0));
		}
	}

	SECTION("Fix extension from header")
	{
		profile->getSettings()->setValue("Save/headerDetection", true);

		auto img = createImage(profile, site);
		ImageDownloader downloader(profile, img, "out.jpg", "tests/resources/tmp", 1, false, false, nullptr, false, false);

		QList<ImageSaveResult> expected;
		expected.append({ QDir::toNativeSeparators("tests/resources/tmp/out.png"), Image::Size::Full, Image::SaveResult::Saved });

		assertDownload(profile, img, &downloader, expected, true);
	}
}
