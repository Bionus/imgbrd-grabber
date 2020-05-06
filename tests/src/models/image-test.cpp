#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QScopedPointer>
#include <QSettings>
#include <QSignalSpy>
#include "loader/token.h"
#include "models/image.h"
#include "models/image-factory.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("Image")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	// Make tmp dir if not already existing
	QDir tmp("tests/resources/");
	if (!tmp.exists("tmp")) {
		tmp.mkdir("tmp");
	}

	QFile::remove("tests/resources/md5s.txt");

	QMap<QString, QString> details;
	details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	details["ext"] = "jpg";
	details["author"] = "superauthor";
	details["status"] = "tested";
	details["filename"] = "";
	details["folder"] = "";
	details["search"] = "testing well";
	details["id"] = "7331";
	details["score"] = "21";
	details["parent_id"] = "1337";
	details["file_size"] = "1234567";
	details["creator_id"] = "1234";
	details["has_children"] = "true";
	details["has_note"] = "true";
	details["has_comments"] = "true";
	details["file_url"] = "http://test.com/img/oldfilename.jpg?123456";
	details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	details["page_url"] = "/posts/7331";
	details["width"] = "800";
	details["height"] = "600";
	details["source"] = "http://google.com/toto/toto.jpg";
	details["tags_general"] = "tag1 tag2 tag3 ";
	details["tags_artist"] = "artist1 ";
	details["tags_copyright"] = "copyright1 copyright2 ";
	details["tags_character"] = "character1 character2 ";
	details["tags_model"] = "model1 ";
	details["created_at"] = "1471513944";
	details["rating"] = "safe";
	details["file_size"] = "358400";
	details["file_size"] = "358400";

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	auto settings = profile->getSettings();
	settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Coloring/Fonts/copyrights", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Coloring/Fonts/characters", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Coloring/Fonts/generals", ",8.25,-1,5,50,0,0,0,0,0");
	settings->setValue("Save/md5Duplicates", "save");
	settings->setValue("Save/md5DuplicatesSameDir", "save");

	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	auto img = ImageFactory::build(site, details, profile);

	SECTION("Constructor")
	{
		QSharedPointer<Image> img;

		// Default
		img = QSharedPointer<Image>(new Image());
		REQUIRE(img->url() == QUrl());

		// Without parent site
		img = ImageFactory::build(nullptr, details, profile);
		REQUIRE(static_cast<int>(img->id()) == 0);

		// With a given page URL
		details["page_url"] = "https://test.com/view/7331";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->pageUrl().toString() == QString("https://test.com/view/7331"));

		// CreatedAt from ISO time
		details.remove("created_at");
		details["date"] = "2016-08-26T16:26:30+01:00";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->createdAt().toString("yyyy-MM-dd HH:mm:ss") == QString("2016-08-26 16:26:30"));
	}

	SECTION("Copy")
	{
		Image clone = *img;

		REQUIRE(clone.tokens(profile) == img->tokens(profile));
		REQUIRE(clone.parentSite() == img->parentSite());
		REQUIRE(clone.parentUrl() == img->parentUrl());
	}

	SECTION("HasTag")
	{
		REQUIRE(img->hasTag("tag1"));
		REQUIRE(img->hasTag("character1"));
		REQUIRE(img->hasTag("tag2"));
		REQUIRE(!img->hasTag("tag7"));
		REQUIRE(!img->hasTag("copyright3"));
	}


	/*SECTION("Md5FromFile")
	{
		details.remove("md5");
		img = ImageFactory::build(site, details, profile);
		img->setSavePath("tests/resources/image_1x1.png");

		REQUIRE(img->md5() == QString("956ddde86fb5ce85218b21e2f49e5c50"));
	}*/

	SECTION("Value")
	{
		// Guess from image size
		REQUIRE(img->value() == 800 * 600);

		// Even with a tag, still use image size if possible
		details["tags_general"] = "lowres";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->value() == 800 * 600);

		// Default value if nothing is given
		details.remove("width");
		details.remove("height");
		details["tags_general"] = "";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->value() == 1200 * 900);

		details["tags_general"] = "incredibly_absurdres";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->value() == 10000 * 10000);

		details["tags_general"] = "absurdres";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->value() == 3200 * 2400);

		details["tags_general"] = "highres";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->value() == 1600 * 1200);

		details["tags_general"] = "lowres";
		img = ImageFactory::build(site, details, profile);
		REQUIRE(img->value() == 500 * 500);
	}

	SECTION("LoadDetails")
	{
		// Load details
		QSignalSpy spy(img.data(), SIGNAL(finishedLoadingTags()));
		img->loadDetails();
		REQUIRE(spy.wait());

		// Compare result
		QList<Tag> tags = img->tags();
		REQUIRE(tags.count() == 26);
		REQUIRE(tags[0].text() == QString("to_heart_2"));
		REQUIRE(tags[0].type().name() == QString("copyright"));
		REQUIRE(tags[0].count() == 6100);
		REQUIRE(tags[1].text() == QString("kousaka_tamaki"));
		REQUIRE(tags[1].type().name() == QString("character"));
		REQUIRE(tags[1].count() == 2100);
		REQUIRE(tags[2].text() == QString("date_(senpen)"));
		REQUIRE(tags[2].type().name() == QString("artist"));
		REQUIRE(tags[2].count() == 256);
		REQUIRE(tags[3].text() == QString("1girl"));
		REQUIRE(tags[3].type().name() == QString("general"));
		REQUIRE(tags[3].count() == 2125000);
	}
	SECTION("LoadDetailsAbort")
	{
		QSignalSpy spy(img.data(), SIGNAL(finishedLoadingTags()));
		img->loadDetails();
		img->abortTags();
		REQUIRE(!spy.wait(1000));
	}

	SECTION("LoadDetailsImageUrl")
	{
		details.remove("file_url");
		img = ImageFactory::build(site, details, profile);

		// Load details
		QSignalSpy spy(img.data(), SIGNAL(finishedLoadingTags()));
		img->loadDetails();
		REQUIRE(spy.wait());

		// Compare result
		REQUIRE(img->url().fileName() == QString("__kousaka_tamaki_to_heart_2_drawn_by_date_senpen__0cc748f006b9636f0c268250ea157995.jpg"));
	}

	SECTION("Save")
	{
		// Delete already existing
		QFile file("tests/resources/tmp/7331.jpg");
		if (file.exists()) {
			file.remove();
		}

		img->setSavePath("tests/resources/image_1x1.png");
		QMap<QString, Image::SaveResult> res = img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));

		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::Saved);
		REQUIRE(file.exists());
		file.remove();
	}
	#ifdef Q_OS_WIN
	SECTION("SaveError")
	{
		QString path = "Z:/../tests/resources/tmp/";

		img->setSavePath("tests/resources/image_1x1.png");
		QMap<QString, Image::SaveResult> res = img->save(QString("%id%.%ext%"), path);

		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::Error);
	}
	#endif
	SECTION("SaveAlreadyExists")
	{
		// Create file if not exists
		QFile file("tests/resources/tmp/7331.jpg");
		REQUIRE(file.open(QFile::Truncate | QFile::WriteOnly));

		img->setSavePath("tests/resources/image_1x1.png");
		QMap<QString, Image::SaveResult> res = img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));

		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::AlreadyExistsDisk);
	}
	SECTION("SaveDuplicate")
	{
		// Delete already existing
		QFile file("tests/resources/tmp/7331.jpg");
		if (file.exists()) {
			file.remove();
		}

		img->setSavePath("tests/resources/image_1x1.png");
		QMap<QString, Image::SaveResult> res;

		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/source.png");
		profile->addMd5(img->md5(), "tests/resources/tmp/source.png");

		settings->setValue("Save/md5Duplicates", "ignore");
		settings->setValue("Save/md5DuplicatesSameDir", "ignore");
		res = img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));
		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::AlreadyExistsMd5);
		REQUIRE(!file.exists());

		settings->setValue("Save/md5Duplicates", "copy");
		settings->setValue("Save/md5DuplicatesSameDir", "copy");
		res = img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));
		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::Copied);
		REQUIRE(file.exists());
		REQUIRE(QFile("tests/resources/tmp/source.png").exists());
		file.remove();

		settings->setValue("Save/md5Duplicates", "move");
		settings->setValue("Save/md5DuplicatesSameDir", "move");
		res = img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));
		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::Moved);
		REQUIRE(file.exists());
		REQUIRE(!QFile("tests/resources/tmp/source.png").exists());
		file.remove();
	}

	SECTION("SaveLog")
	{
		// Delete already existing
		QFile file("tests/resources/tmp/7331.jpg");
		if (file.exists()) {
			file.remove();
		}
		QFile logFile("tests/resources/tmp/savelog.txt");
		if (logFile.exists()) {
			logFile.remove();
		}

		settings->setValue("LogFiles/0/locationType", 1);
		settings->setValue("LogFiles/0/uniquePath", logFile.fileName());
		settings->setValue("LogFiles/0/content", "id: %id%");

		img->setSavePath("tests/resources/image_1x1.png");
		QMap<QString, Image::SaveResult> res = img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));

		REQUIRE(res.count() == 1);
		REQUIRE(res.first() == Image::Saved);
		REQUIRE(file.exists());
		REQUIRE(logFile.exists());

		REQUIRE(logFile.open(QFile::ReadOnly | QFile::Text));
		REQUIRE(QString(logFile.readAll()) == QString("id: 7331"));
		logFile.close();

		file.remove();
		logFile.remove();

		settings->remove("LogFiles/0/locationType");
		settings->remove("LogFiles/0/uniquePath");
		settings->remove("LogFiles/0/content");
	}

	SECTION("SetUrl")
	{
		QUrl url("http://google.fr");

		REQUIRE(img->url() != url);
		img->setUrl(url);
		REQUIRE(img->url() == url);
	}

	SECTION("GrabberFavoritedToken")
	{
		auto tokens = img->tokens(profile);
		REQUIRE(!tokens["grabber"].value().toStringList().contains("favorited"));

		Favorite fav("tag2");
		profile->addFavorite(fav);
		img->refreshTokens();
		tokens = img->tokens(profile);
		REQUIRE(tokens["grabber"].value().toStringList().contains("favorited"));
		profile->removeFavorite(fav);
	}

	SECTION("Serialization")
	{
		QJsonObject json;
		img->write(json);

		Image dest(profile);
		dest.read(json, profile->getSites());

		REQUIRE(dest.id() == img->id());
		REQUIRE(dest.md5() == img->md5());
		REQUIRE(dest.fileSize() == img->fileSize());

		REQUIRE(dest.url() == img->url());
		REQUIRE(dest.url(Image::Size::Sample) == img->url(Image::Size::Sample));
		REQUIRE(dest.url(Image::Size::Thumbnail) == img->url(Image::Size::Thumbnail));
	}
}
