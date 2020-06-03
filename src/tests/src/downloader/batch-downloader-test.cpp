#include <QDir>
#include <QFile>
#include <QScopedPointer>
#include <QSettings>
#include <QSignalSpy>
#include "catch.h"
#include "custom-network-access-manager.h"
#include "downloader/batch-downloader.h"
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "models/profile.h"
#include "source-helpers.h"


void waitForFinished(BatchDownloader *downloader)
{
	QSignalSpy spy(downloader, SIGNAL(finished()));
	downloader->start();
	REQUIRE(spy.wait());
}

TEST_CASE("BatchDownloader")
{
	QDir("tests/resources/").mkdir("tmp");

	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	// Force HTML source
	QSettings siteSettings("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/settings.ini", QSettings::IniFormat);
	siteSettings.clear();
	siteSettings.setValue("sources/usedefault", false);
	siteSettings.setValue("sources/source_1", "html");
	siteSettings.sync();

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	QDir dir("tests/resources/tmp/");
	for (const QString &file : dir.entryList(QDir::Files)) {
		dir.remove(file);
	}

	SECTION("Single image download")
	{
		auto img = QSharedPointer<Image>(new Image(site, {{ "file_url", "http://test.com/img/filename.jpg" }}, profile));
		DownloadQueryImage query(img, site, "out.jpg", "tests/resources/tmp");

		SECTION("Valid")
		{
			BatchDownloader downloader(&query, profile);
			waitForFinished(&downloader);

			QFile f("tests/resources/tmp/out.jpg");
			REQUIRE(f.exists());
			REQUIRE(f.remove());

			REQUIRE(downloader.downloadedCount(BatchDownloader::Downloaded) == 1);
			REQUIRE(downloader.totalCount() == 1);
		}

		SECTION("Image not found")
		{
			// More than once because of the extension rotator
			for (int i = 0; i < 7; ++i)
				CustomNetworkAccessManager::NextFiles.append("404");

			BatchDownloader downloader(&query, profile);
			waitForFinished(&downloader);

			QFile f("tests/resources/tmp/out.jpg");
			REQUIRE(!f.exists());

			REQUIRE(downloader.downloadedCount(BatchDownloader::NotFound) == 1);
			REQUIRE(downloader.totalCount() == 1);
		}
	}

	SECTION("Group download")
	{
		QSettings *settings = profile->getSettings();
		settings->setValue("packing_size", 2);

		int total = 5;
		DownloadQueryGroup query(QStringList() << "rating:safe", 1, 20, total, QStringList(), true, site, "%count%.%ext%", "tests/resources/tmp");

		SECTION("Valid")
		{
			CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/results.html");

			total = GENERATE(2, 5);
			query.total = total;

			BatchDownloader downloader(&query, profile);
			waitForFinished(&downloader);

			REQUIRE(downloader.downloadedCount() == total);
			REQUIRE(downloader.downloadedCount(BatchDownloader::Downloaded) == total);
			REQUIRE(downloader.totalCount() == total);
		}

		SECTION("No results")
		{
			CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/results.xml"); // Will cause a parsing error

			BatchDownloader downloader(&query, profile);
			waitForFinished(&downloader);

			REQUIRE(downloader.downloadedCount() == 0);
		}

		SECTION("Abort")
		{
			SECTION("Before start")
			{
				CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/results.html");

				BatchDownloader downloader(&query, profile);
				downloader.abort();
				waitForFinished(&downloader);

				REQUIRE(downloader.downloadedCount() == total);
				REQUIRE(downloader.downloadedCount(BatchDownloader::Downloaded) == total);
				REQUIRE(downloader.totalCount() == total);
			}

			SECTION("After finished")
			{
				CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/results.html");

				BatchDownloader downloader(&query, profile);
				waitForFinished(&downloader);
				downloader.abort();

				REQUIRE(downloader.downloadedCount() == total);
				REQUIRE(downloader.downloadedCount(BatchDownloader::Downloaded) == total);
				REQUIRE(downloader.totalCount() == total);
			}
		}
	}
}
