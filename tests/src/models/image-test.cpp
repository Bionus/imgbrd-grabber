#include <QtTest>
#include "test-suite.h"
#include "image-test.h"
#include "functions.h"


void ImageTest::init()
{
	// Make tmp dir if not already existing
	QDir tmp("tests/resources/");
	if (!tmp.exists("tmp"))
		tmp.mkdir("tmp");

	QFile::remove("tests/resources/md5s.txt");

	m_details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	m_details["ext"] = "jpg";
	m_details["author"] = "superauthor";
	m_details["status"] = "tested";
	m_details["filename"] = "";
	m_details["folder"] = "";
	m_details["search"] = "testing well";
	m_details["id"] = "7331";
	m_details["score"] = "21";
	m_details["parent_id"] = "1337";
	m_details["file_size"] = "1234567";
	m_details["creator_id"] = "1234";
	m_details["has_children"] = "true";
	m_details["has_note"] = "true";
	m_details["has_comments"] = "true";
	m_details["file_url"] = "http://test.com/img/oldfilename.jpg?123456";
	m_details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	m_details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	m_details["page_url"] = "";
	m_details["width"] = "800";
	m_details["height"] = "600";
	m_details["source"] = "http://google.com/toto/toto.jpg";
	m_details["tags_general"] = "tag1 tag2 tag3 ";
	m_details["tags_artist"] = "artist1 ";
	m_details["tags_copyright"] = "copyright1 copyright2 ";
	m_details["tags_character"] = "character1 character2 ";
	m_details["tags_model"] = "model1 ";
	m_details["created_at"] = "1471513944";
	m_details["rating"] = "safe";
	m_details["file_size"] = "358400";
	m_details["file_size"] = "358400";

	m_profile = new Profile("tests/resources/");
	m_settings = m_profile->getSettings();
	m_settings->setValue("Coloring/Fonts/artists", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Fonts/copyrights", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Fonts/characters", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Coloring/Fonts/generals", ",8.25,-1,5,50,0,0,0,0,0");
	m_settings->setValue("Save/md5Duplicates", "save");

	m_source = new Source(&profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
	m_img = new Image(m_site, m_details, m_profile);
}

void ImageTest::cleanup()
{
	m_settings->deleteLater();
	m_site->deleteLater();
	m_img->deleteLater();
}


void ImageTest::testConstructor()
{
	Image *img;

	// Default
	img = new Image();
	QCOMPARE(img->url(), QString());
	img->deleteLater();

	// Without parent site
	img = new Image(nullptr, m_details, m_profile);
	QCOMPARE(img->details().isEmpty(), true);
	img->deleteLater();

	// With a given page URL
	m_details["page_url"] = "https://test.com/view/7331";
	img = new Image(m_site, m_details, m_profile);
	QCOMPARE(img->pageUrl().toString(), QString("https://test.com/view/7331"));
	img->deleteLater();

	// CreatedAt from ISO time
	m_details.remove("created_at");
	m_details["date"] = "2016-08-26T16:26:30+01:00";
	img = new Image(m_site, m_details, m_profile);
	QCOMPARE(img->createdAt().toString("yyyy-MM-dd HH:mm:ss"), QString("2016-08-26 16:26:30"));
	img->deleteLater();
}

void ImageTest::testCopy()
{
	Image clone = *m_img;

	QCOMPARE(clone.id(), m_img->id());
	QCOMPARE(clone.md5(), m_img->md5());
	QCOMPARE(clone.tags(), m_img->tags());
	QCOMPARE(clone.author(), m_img->author());
	QCOMPARE(clone.status(), m_img->status());
	QCOMPARE(clone.rating(), m_img->rating());
	QCOMPARE(clone.source(), m_img->source());
	QCOMPARE(clone.site(), m_img->site());
	QCOMPARE(clone.parentSite(), m_img->parentSite());
	QCOMPARE(clone.filename(), m_img->filename());
	QCOMPARE(clone.folder(), m_img->folder());
	QCOMPARE(clone.pools().count(), m_img->pools().count());
	QCOMPARE(clone.fileSize(), m_img->fileSize());
	QCOMPARE(clone.score(), m_img->score());
	QCOMPARE(clone.parentId(), m_img->parentId());
	QCOMPARE(clone.width(), m_img->width());
	QCOMPARE(clone.height(), m_img->height());
	QCOMPARE(clone.authorId(), m_img->authorId());
	QCOMPARE(clone.createdAt(), m_img->createdAt());
	QCOMPARE(clone.hasChildren(), m_img->hasChildren());
	QCOMPARE(clone.hasNote(), m_img->hasNote());
	QCOMPARE(clone.hasComments(), m_img->hasComments());
	QCOMPARE(clone.hasScore(), m_img->hasScore());
	QCOMPARE(clone.fileUrl(), m_img->fileUrl());
	QCOMPARE(clone.sampleUrl(), m_img->sampleUrl());
	QCOMPARE(clone.previewUrl(), m_img->previewUrl());
	QCOMPARE(clone.pageUrl(), m_img->pageUrl());
	QCOMPARE(clone.size(), m_img->size());
	QCOMPARE(clone.previewImage(), m_img->previewImage());
	QCOMPARE(clone.page(), m_img->page());
	QCOMPARE(clone.data(), m_img->data());
	QCOMPARE(clone.settings(), m_img->settings());
	QCOMPARE(clone.details(), m_img->details());
	QCOMPARE(clone.search(), m_img->search());
}

void ImageTest::testHasTag()
{
	QCOMPARE(m_img->hasTag("tag1"), true);
	QCOMPARE(m_img->hasTag("character1"), true);
	QCOMPARE(m_img->hasTag("tag2"), true);
	QCOMPARE(m_img->hasTag("tag7"), false);
	QCOMPARE(m_img->hasTag("copyright3"), false);
}
void ImageTest::testHasAnyTag()
{
	QCOMPARE(m_img->hasAnyTag(QStringList() << "tag1" << "tag2"), true);
	QCOMPARE(m_img->hasAnyTag(QStringList() << "tag7" << "tag1"), true);
	QCOMPARE(m_img->hasAnyTag(QStringList() << "tag4" << "tag7"), false);
}
void ImageTest::testHasAllTags()
{
	QCOMPARE(m_img->hasAllTags(QStringList() << "tag1" << "tag2"), true);
	QCOMPARE(m_img->hasAllTags(QStringList() << "tag7" << "tag1"), false);
	QCOMPARE(m_img->hasAllTags(QStringList() << "tag4" << "tag7"), false);
}


void ImageTest::testMd5FromData()
{
	m_details.remove("md5");
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	m_img->setData(QString("test").toLatin1());

	QCOMPARE(m_img->md5(), QString("098f6bcd4621d373cade4e832627b4f6"));
}
void ImageTest::testMd5FromFile()
{
	m_details.remove("md5");
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	m_img->setSavePath("tests/resources/image_1x1.png");

	QCOMPARE(m_img->md5(), QString("956ddde86fb5ce85218b21e2f49e5c50"));
}

void ImageTest::testStylishedTags()
{
	m_profile->getIgnored() = QStringList();
	QStringList tags = m_img->stylishedTags(m_profile);

	QCOMPARE(tags.count(), 9);
	/*QCOMPARE(tags[0], QString("<a href=\"artist1\" style=\"color:#aa0000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">artist1</a>"));
	QCOMPARE(tags[1], QString("<a href=\"character1\" style=\"color:#00aa00; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">character1</a>"));
	QCOMPARE(tags[7], QString("<a href=\"tag2\" style=\"color:#000000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag2</a>"));*/

	m_settings->setValue("blacklistedtags", "character1 tag1");
	m_profile->getIgnored() = QStringList() << "copyright1" << "tag2";
	tags = m_img->stylishedTags(m_profile);

	QCOMPARE(tags.count(), 9);
	/*QCOMPARE(tags[1], QString("<a href=\"character1\" style=\"color:#000000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">character1</a>"));
	QCOMPARE(tags[3], QString("<a href=\"copyright1\" style=\"color:#999999; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">copyright1</a>"));
	QCOMPARE(tags[8], QString("<a href=\"tag3\" style=\"color:#000000; font-family:''; font-size:8pt; font-style:normal; font-weight:400; text-decoration:none;\">tag3</a>"));*/
}

void ImageTest::testUnload()
{
	m_img->setData(QString("test").toLatin1());
	QCOMPARE(m_img->data().isEmpty(), false);

	m_img->unload();
	QCOMPARE(m_img->data().isEmpty(), true);
}

void ImageTest::testBlacklisted()
{
	// Basic
	QCOMPARE(m_img->blacklisted(QStringList() << "tag8" << "tag7"), QStringList());
	QCOMPARE(m_img->blacklisted(QStringList() << "tag1" << "tag7"), QStringList() << "tag1");
	QCOMPARE(m_img->blacklisted(QStringList() << "character1" << "artist1"), QStringList() << "character1" << "artist1");

	// Invert
	QCOMPARE(m_img->blacklisted(QStringList() << "tag8" << "tag7", false), QStringList() << "tag8" << "tag7");
	QCOMPARE(m_img->blacklisted(QStringList() << "tag1" << "tag7", false), QStringList() << "tag7");
	QCOMPARE(m_img->blacklisted(QStringList() << "character1" << "artist1", false), QStringList());
}

void ImageTest::testMatchTag()
{
	// Basic
	QCOMPARE(m_img->match("tag1"), QString());
	QCOMPARE(m_img->match("character1"), QString());
	QCOMPARE(m_img->match("tag7"), QString("image does not contains \"tag7\""));

	// Minus
	QCOMPARE(m_img->match("-tag1"), QString("image contains \"tag1\""));
	QCOMPARE(m_img->match("-character1"), QString("image contains \"character1\""));
	QCOMPARE(m_img->match("-tag7"), QString());

	// Invert
	QCOMPARE(m_img->match("tag1", true), QString("image contains \"tag1\""));
	QCOMPARE(m_img->match("character1", true), QString("image contains \"character1\""));
	QCOMPARE(m_img->match("tag7", true), QString());

	// Invert minus
	QCOMPARE(m_img->match("-tag1", true), QString());
	QCOMPARE(m_img->match("-character1", true), QString());
	QCOMPARE(m_img->match("-tag7", true), QString("image does not contains \"tag7\""));
}

void ImageTest::testMatchUnknown()
{
	QCOMPARE(m_img->match("toto:test").startsWith("unknown type \"toto\""), true);
}

void ImageTest::testMatchMathematical()
{
	// Basic
	QCOMPARE(m_img->match("id:>1000"), QString());
	QCOMPARE(m_img->match("id:<=1000"), QString("image's id does not match"));
	QCOMPARE(m_img->match("id:>=0", true), QString("image's id match"));

	// Other types
	QCOMPARE(m_img->match("width:..1000"), QString());
	QCOMPARE(m_img->match("height:500.."), QString());
	QCOMPARE(m_img->match("score:10..30"), QString());
	QCOMPARE(m_img->match("mpixels:<1000000"), QString());
	QCOMPARE(m_img->match("filesize:358400"), QString());
}

void ImageTest::testMatchDate()
{
	QCOMPARE(m_img->match("date:>08/16/2016"), QString());
	QCOMPARE(m_img->match("date:>=2016-08-16"), QString());
	QCOMPARE(m_img->match("date:<08/20/2016"), QString());
	QCOMPARE(m_img->match("date:<=2016-08-20"), QString());
	QCOMPARE(m_img->match("date:..08/20/2016"), QString());
	QCOMPARE(m_img->match("date:2016-08-16.."), QString());
	QCOMPARE(m_img->match("date:08/16/2016..2016-08-20"), QString());
	QCOMPARE(m_img->match("date:2016-08-18"), QString());

	// Invalid date
	QCOMPARE(m_img->match("date:someday"), QString("image's date does not match"));
}

void ImageTest::testMatchRating()
{
	// Basic
	QCOMPARE(m_img->match("rating:safe"), QString());
	QCOMPARE(m_img->match("rating:explicit"), QString("image is not \"explicit\""));

	// Short versions
	QCOMPARE(m_img->match("rating:s"), QString());
	QCOMPARE(m_img->match("rating:e"), QString("image is not \"explicit\""));

	// Invert
	QCOMPARE(m_img->match("rating:safe", true), QString("image is \"safe\""));
	QCOMPARE(m_img->match("rating:explicit", true), QString());
}

void ImageTest::testMatchSource()
{
	// Full
	QCOMPARE(m_img->match("source:http://google.com/toto/toto.jpg"), QString());
	QCOMPARE(m_img->match("source:http://test.fr/toto/toto.jpg"), QString("image's source does not starts with \"http://test.fr/toto/toto.jpg\""));

	// Short
	QCOMPARE(m_img->match("source:http://google.com"), QString());
	QCOMPARE(m_img->match("source:http://test.fr"), QString("image's source does not starts with \"http://test.fr\""));

	// Invert
	QCOMPARE(m_img->match("source:http://google.com", true), QString("image's source starts with \"http://google.com\""));
	QCOMPARE(m_img->match("source:http://test.fr", true), QString());
}

void ImageTest::testFilter()
{
	QStringList filters;

	// No match
	filters = m_img->filter(QStringList() << "id:<=10000" << "width:>100");
	QCOMPARE(filters, QStringList());

	// All match
	filters = m_img->filter(QStringList() << "id:>10000" << "width:<=100");
	QCOMPARE(filters, QStringList() << "image's id does not match" << "image's width does not match");
}

void ImageTest::testValue()
{
	// Guess from image size
	QCOMPARE(m_img->value(), 800 * 600);

	// Even with a tag, still use image size if possible
	m_details["tags_general"] = "lowres";
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	QCOMPARE(m_img->value(), 800 * 600);

	// Default value if nothing is given
	m_details.remove("width");
	m_details.remove("height");
	m_details["tags_general"] = "";
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	QCOMPARE(m_img->value(), 1200 * 900);

	m_details["tags_general"] = "incredibly_absurdres";
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	QCOMPARE(m_img->value(), 10000 * 10000);

	m_details["tags_general"] = "absurdres";
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	QCOMPARE(m_img->value(), 3200 * 2400);

	m_details["tags_general"] = "highres";
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	QCOMPARE(m_img->value(), 1600 * 1200);

	m_details["tags_general"] = "lowres";
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	QCOMPARE(m_img->value(), 500 * 500);
}

void ImageTest::testLoadPreview()
{
	// Load preview
	QSignalSpy spy(m_img, SIGNAL(finishedLoadingPreview()));
	m_img->loadPreview();
	QVERIFY(spy.wait());

	// Compare result
	QCOMPARE(m_img->previewImage().size(), QSize(1, 1));
}
void ImageTest::testLoadPreviewAbort()
{
	QSignalSpy spy(m_img, SIGNAL(finishedLoadingPreview()));
	m_img->loadPreview();
	m_img->abortPreview();
	QVERIFY(!spy.wait(1000));
}

void ImageTest::testLoadImage()
{
	// Load preview
	QSignalSpy spy(m_img, SIGNAL(finishedImage(QNetworkReply::NetworkError,QString)));
	m_img->loadImage();
	QVERIFY(spy.wait());

	// Compare result
	QCOMPARE(m_img->data().isEmpty(), false);
}
void ImageTest::testLoadImageAbort()
{
	QSignalSpy spy(m_img, SIGNAL(finishedImage()));
	m_img->loadImage();
	m_img->abortImage();
	QVERIFY(!spy.wait(1000));
}

void ImageTest::testLoadDetails()
{
	// Load details
	QSignalSpy spy(m_img, SIGNAL(finishedLoadingTags()));
	m_img->loadDetails();
	QVERIFY(spy.wait());

	// Compare result
	QList<Tag> tags = m_img->tags();
	QCOMPARE(tags.count(), 23);
	QCOMPARE(tags[0].text(), QString("to_heart_2"));
	QCOMPARE(tags[0].type().name(), QString("copyright"));
	QCOMPARE(tags[0].count(), 5900);
	QCOMPARE(tags[1].text(), QString("kousaka_tamaki"));
	QCOMPARE(tags[1].type().name(), QString("character"));
	QCOMPARE(tags[1].count(), 2000);
	QCOMPARE(tags[2].text(), QString("date_(senpen)"));
	QCOMPARE(tags[2].type().name(), QString("artist"));
	QCOMPARE(tags[2].count(), 251);
	QCOMPARE(tags[3].text(), QString("1girl"));
	QCOMPARE(tags[3].type().name(), QString("general"));
	QCOMPARE(tags[3].count(), 1679000);
}
void ImageTest::testLoadDetailsAbort()
{
	QSignalSpy spy(m_img, SIGNAL(finishedLoadingTags()));
	m_img->loadDetails();
	m_img->abortTags();
	QVERIFY(!spy.wait(1000));
}

void ImageTest::testLoadDetailsImageUrl()
{
	m_img->deleteLater();
	m_details.remove("file_url");
	m_img = new Image(m_site, m_details, m_profile);

	// Load details
	QSignalSpy spy(m_img, SIGNAL(finishedLoadingTags()));
	m_img->loadDetails();
	QVERIFY(spy.wait());

	// Compare result
	QCOMPARE(m_img->url(), QString("https://danbooru.donmai.us/data/__kousaka_tamaki_to_heart_2_drawn_by_date_senpen__0cc748f006b9636f0c268250ea157995.jpg"));
}

void ImageTest::testPath()
{
	QStringList path;

	// Simple
	path = m_img->path("%md5%.%ext%", "", 0, true, true);
	QCOMPARE(path, QStringList() << "1bc29b36f623ba82aaf6724fd3b16718.jpg");

	// Not simple (settings)
	m_settings->setValue("Save/filename", "%id%.%ext%");
	m_settings->setValue("Save/path", QDir::homePath());
	path = m_img->path("", "", 0, true, false);
	QCOMPARE(path, QStringList() << "7331.jpg");

	// Not simple (details)
	m_settings->setValue("Save/filename", "");
	m_settings->setValue("Save/path", "");
	m_details["filename"] = "%id%.%ext%";
	m_details["folder"] = QDir::homePath();
	m_img->deleteLater();
	m_img = new Image(m_site, m_details, m_profile);
	path = m_img->path("%md5%.%ext%", "", 0, true, false);
	QCOMPARE(path, QStringList() << "7331.jpg");
}

void ImageTest::testSave()
{
	// Delete already existing
	QFile file("tests/resources/tmp/7331.jpg");
	if (file.exists())
		file.remove();

	m_img->setData(QString("test").toLatin1());
	QMap<QString, Image::SaveResult> res = m_img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));

	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::Saved);
	QCOMPARE(file.exists(), true);
	file.remove();
}
#ifdef Q_OS_WIN
void ImageTest::testSaveError()
{
	QString path = "Z:/../tests/resources/tmp/";

	m_img->setData(QString("test").toLatin1());
	QMap<QString, Image::SaveResult> res = m_img->save(QString("%id%.%ext%"), path);

	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::Error);
}
#endif
void ImageTest::testSaveAlreadyExists()
{
	// Create file if not exists
	QFile file("tests/resources/tmp/7331.jpg");
	if (!file.open(QFile::Truncate | QFile::WriteOnly))
		QFAIL("Cannot create file");

	m_img->setData(QString("test").toLatin1());
	QMap<QString, Image::SaveResult> res = m_img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));

	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::AlreadyExists);
}
void ImageTest::testSaveDuplicate()
{
	// Delete already existing
	QFile file("tests/resources/tmp/7331.jpg");
	if (file.exists())
		file.remove();

	m_img->setData(QString("test").toLatin1());
	QMap<QString, Image::SaveResult> res;

	QFile::copy("tests/resources/image_1x1.png", "tests/resources/tmp/source.png");
	m_profile->addMd5(m_img->md5(), "tests/resources/tmp/source.png");

	m_settings->setValue("Save/md5Duplicates", "ignore");
	res = m_img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));
	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::Ignored);
	QCOMPARE(file.exists(), false);

	m_settings->setValue("Save/md5Duplicates", "copy");
	res = m_img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));
	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::Copied);
	QCOMPARE(file.exists(), true);
	QCOMPARE(QFile("tests/resources/tmp/source.png").exists(), true);
	file.remove();

	m_settings->setValue("Save/md5Duplicates", "move");
	res = m_img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));
	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::Moved);
	QCOMPARE(file.exists(), true);
	QCOMPARE(QFile("tests/resources/tmp/source.png").exists(), false);
	file.remove();
}

void ImageTest::testSaveLog()
{
	// Delete already existing
	QFile file("tests/resources/tmp/7331.jpg");
	if (file.exists())
		file.remove();
	QFile logFile("tests/resources/tmp/savelog.txt");
	if (logFile.exists())
		logFile.remove();

	m_settings->setValue("LogFiles/0/locationType", 1);
	m_settings->setValue("LogFiles/0/uniquePath", logFile.fileName());
	m_settings->setValue("LogFiles/0/content", "id: %id%");

	m_img->setData(QString("test").toLatin1());
	QMap<QString, Image::SaveResult> res = m_img->save(QString("%id%.%ext%"), QString("tests/resources/tmp/"));

	QCOMPARE(res.count(), 1);
	QCOMPARE(res.first(), Image::Saved);
	QCOMPARE(file.exists(), true);
	QCOMPARE(logFile.exists(), true);

	if (!logFile.open(QFile::ReadOnly | QFile::Text))
		QFAIL("Could not open text file");
	QCOMPARE(QString(logFile.readAll()), QString("id: 7331"));
	logFile.close();

	file.remove();
	logFile.remove();

	m_settings->remove("LogFiles/0/locationType");
	m_settings->remove("LogFiles/0/uniquePath");
	m_settings->remove("LogFiles/0/content");
}

void ImageTest::testSetUrl()
{
	QString url = "http://google.fr";

	QCOMPARE(m_img->url() != url, true);
	m_img->setUrl(url);
	QCOMPARE(m_img->url(), url);
}


static ImageTest instance;
