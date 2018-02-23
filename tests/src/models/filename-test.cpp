#include "filename-test.h"
#include <QtTest>


void FilenameTest::init()
{
	// Make tmp dir if not already existing
	QDir tmp("tests/resources/");
	if (!tmp.exists("tmp"))
		tmp.mkdir("tmp");

	m_details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	m_details["ext"] = "jpg";
	m_details["author"] = "superauthor";
	m_details["status"] = "tested";
	m_details["filename"] = "oldfilename";
	m_details["search"] = "testing well";
	m_details["id"] = "7331";
	m_details["score"] = "21";
	m_details["parent_id"] = "1337";
	m_details["file_size"] = "1234567";
	m_details["creator_id"] = "1234";
	m_details["has_children"] = "true";
	m_details["has_note"] = "true";
	m_details["has_comments"] = "true";
	m_details["file_url"] = "http://test.com/img/oldfilename.jpg";
	m_details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	m_details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	m_details["width"] = "800";
	m_details["height"] = "600";
	m_details["source"] = "http://google.com/";
	m_details["tags_general"] = "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3";
	m_details["tags_artist"] = "artist1";
	m_details["tags_copyright"] = "copyright1 copyright2";
	m_details["tags_character"] = "character1 character2";
	m_details["tags_species"] = "";
	m_details["created_at"] = "1471513944";
	m_details["rating"] = "safe";

	m_profile = new Profile("tests/resources/settings.ini");
	m_settings = m_profile->getSettings();
	m_settings->clear();
	m_settings->setValue("ignoredtags", "");
	m_settings->setValue("Save/separator", " ");
	m_settings->setValue("Save/character_value", "group");
	m_settings->setValue("Save/character_multiple", "replaceAll");
	m_settings->setValue("Save/copyright_value", "crossover");
	m_settings->setValue("Save/copyright_multiple", "replaceAll");
	m_settings->setValue("Save/character_empty", "unknown");
	m_settings->setValue("Save/replaceblanks", true);

	m_source = new Source(m_profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
	m_img = new Image(m_site, m_details, m_profile);
}

void FilenameTest::cleanup()
{
	delete m_profile;
	m_site->deleteLater();
	m_img->deleteLater();
}


void FilenameTest::testDefaultConstructor()
{
	Filename fn;

	QCOMPARE(fn.getFormat().isEmpty(), true);
}

void FilenameTest::testGetFormat()
{
	QString format = "%md5%.%ext%";
	Filename fn(format);

	QCOMPARE(fn.getFormat(), format);
}

void FilenameTest::testSetFormat()
{
	QString format = "%md5%.%ext%";
	Filename fn("%id%.%ext%");
	fn.setFormat(format);

	QCOMPARE(fn.getFormat(), format);
}

void FilenameTest::testPathSimple()
{
	assertPath("%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathComplex()
{
	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/group/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathKeepAll()
{
	m_settings->setValue("Save/character_multiple", "keepAll");

	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/character1 character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathKeepN()
{
	m_settings->setValue("Save/character_multiple", "keepN");
	m_settings->setValue("Save/character_multiple_keepN", 1);

	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/character1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathKeepNThenAdd()
{
	m_settings->setValue("Save/character_multiple", "keepNThenAdd");
	m_settings->setValue("Save/character_multiple_keepNThenAdd_keep", 1);
	m_settings->setValue("Save/character_multiple_keepNThenAdd_add", " (and %count% of %total%)");
	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/character1 (and 1 of 2)/1bc29b36f623ba82aaf6724fd3b16718.jpg");

	m_settings->setValue("Save/character_multiple_keepNThenAdd_keep", 2);
	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/character1 character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathIgnoredTags()
{
	m_img->deleteLater();
	m_settings->setValue("ignoredtags", "character1");
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");

	m_img->deleteLater();
	m_settings->setValue("ignoredtags", "character*");
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/unknown/1bc29b36f623ba82aaf6724fd3b16718.jpg");

	m_img->deleteLater();
	m_settings->setValue("Save/character_empty", "");
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
			   "artist1/crossover/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathEmptyDirs()
{
	assertPath("%artist%/%test%/%md5%.%ext%",
			   "artist1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathEmptyDirsNetworkDrive()
{
	assertPath("%md5%.%ext%",
			   "//NetworkDrive/Grabber/1bc29b36f623ba82aaf6724fd3b16718.jpg",
			   "//NetworkDrive/Grabber", true, true);
}
void FilenameTest::testPathKeptTokens()
{
	assertPath("%artist%/%path%/%md5%.%ext%",
			   "artist1/%path%/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathFull()
{
	assertPath("%md5%.%ext%",
			   "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
			   "tests/directory/",
			   true, true);
	assertPath("%md5%.%ext%",
			   "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
			   "tests/directory",
			   true, true);
	assertPath("/%md5%.%ext%",
			   "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
			   "tests/directory/",
			   true, true);
	assertPath("/%md5%.%ext%",
			   "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
			   "tests/directory",
			   true, true);
}

void FilenameTest::testPathSimpleJavascript()
{
	assertPath("javascript:md5 + '.' + ext", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testPathJavascriptToken()
{
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	assertPath("javascript:copyright", "copyright1 copyright2");
}

void FilenameTest::testPathJavascriptArray()
{
	// TODO(Bionus): make the "keepAll" unnecessary for this array
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	assertPath("javascript:copyrights.join('-')", "copyright1-copyright2");
}

void FilenameTest::testPathJavascriptDate()
{
	assertPath("javascript:date.toISOString().slice(0, 10)", "2016-08-18");
}

void FilenameTest::testPathInvalidJavascript()
{
	assertPath("javascript:'", QStringList());
}

void FilenameTest::testExpandTagSimple()
{
	assertExpand("<image contains the tag \"tag1\"><\"unknown\" is one of the image tags> %md5%.%ext%",
				 "image contains the tag tag1 %md5%.%ext%");
	assertExpand("<image contains the tag \"unknown\"><\"tag2\" is one of the image tags> %md5%.%ext%",
				 "tag2 is one of the image tags %md5%.%ext%");
	assertExpand("<image contains the tag \"tag1\"><\"tag2\" is one of the image tags> %md5%.%ext%",
				 "image contains the tag tag1tag2 is one of the image tags %md5%.%ext%");
	assertExpand("<image contains the tag \"unknown1\"><\"unknown2\" is one of the image tags> %md5%.%ext%",
				 " %md5%.%ext%");
}
void FilenameTest::testExpandTagWithInvalidCharacter()
{
	assertExpand("<\"fate/stay_night\"/>%md5%.%ext%", "%md5%.%ext%");

	m_img->deleteLater();
	m_details["tags_copyright"] = "fate/stay_night";
	m_img = new Image(m_site, m_details, m_profile);

	assertExpand("<\"fate/stay_night\"/>%md5%.%ext%", "fate_stay_night/%md5%.%ext%");
}
void FilenameTest::testExpandTagInvert()
{
	assertExpand("<image does not contain the tag !\"tag1\"><!\"unknown\" is not one of the image tags> %md5%.%ext%",
				 "unknown is not one of the image tags %md5%.%ext%");
	assertExpand("<image does not contain the tag !\"unknown\"><!\"tag2\" is not one of the image tags> %md5%.%ext%",
				 "image does not contain the tag unknown %md5%.%ext%");
}
void FilenameTest::testExpandTagMultiple()
{
	assertExpand("<\"tag1\" \"tag2\"/>%md5%.%ext%", "tag1 tag2/%md5%.%ext%");
	assertExpand("<\"tag1\" \"tag4\"/>%md5%.%ext%", "%md5%.%ext%");

	assertExpand("<\"tag1\" !\"tag4\"/>%md5%.%ext%", "tag1 tag4/%md5%.%ext%");
	assertExpand("<\"tag1\" !\"tag2\"/>%md5%.%ext%", "%md5%.%ext%");
}
void FilenameTest::testExpandTagIgnore()
{
	assertExpand("<\"tag1\"folder1/>%md5%.%ext%", "tag1folder1/%md5%.%ext%");
	assertExpand("<-\"tag1\"folder1/>%md5%.%ext%", "folder1/%md5%.%ext%");

	assertExpand("<\"tag1\"\"tag2\"folder1/>%md5%.%ext%", "tag1tag2folder1/%md5%.%ext%");
	assertExpand("<-\"tag1\"-\"tag2\"folder1/>%md5%.%ext%", "folder1/%md5%.%ext%");

	assertExpand("<\"tag1\" \"tag2\"/>%md5%.%ext%", "tag1 tag2/%md5%.%ext%");
	assertExpand("<\"tag1\"-\"tag2\"/>%md5%.%ext%", "tag1/%md5%.%ext%");
	assertExpand("<-\"tag1\"\"tag2\"/>%md5%.%ext%", "tag2/%md5%.%ext%");
	assertExpand("<-\"tag1\"-\"tag2\"/>%md5%.%ext%", "/%md5%.%ext%");
}
void FilenameTest::testExpandTokenSimple()
{
	assertExpand("image - <%artist% some text><text %nothing%> %md5%.%ext%",
				 "image - %artist% some text %md5%.%ext%");
}
void FilenameTest::testExpandTokenInvert()
{
	assertExpand("image - <!%artist% some text><text !%nothing%> %md5%.%ext%",
				 "image - text  %md5%.%ext%");
}
void FilenameTest::testExpandTokenComplex()
{
	/*assertExpand("image - <%artist% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%",
				 "image - %artist% some text  test %md5%.%ext%");
	assertExpand("image - <%model% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%",
				 "image -  %md5%.%ext%");*/
}
void FilenameTest::testExpandMultipleMixed()
{
	assertExpand("<\"tag1\" %artist%/>%md5%.%ext%", "tag1 %artist%/%md5%.%ext%");
	assertExpand("<\"tag1\" %nothing%/>%md5%.%ext%", "%md5%.%ext%");

	assertExpand("<\"tag1\"!%nothing%/>%md5%.%ext%", "tag1/%md5%.%ext%");
	assertExpand("<\"tag1\"!%artist%/>%md5%.%ext%", "%md5%.%ext%");

	assertExpand("<\"tag1\"-%artist%/>%md5%.%ext%", "tag1/%md5%.%ext%");
	assertExpand("<-\"tag1\"%artist%/>%md5%.%ext%", "%artist%/%md5%.%ext%");
}
void FilenameTest::testExpandEscaping()
{
	assertExpand("<<Value>>%md5%<</Value>>",
				 "<Value>%md5%</Value>");
}

void FilenameTest::testPathOptionMax()
{
	assertPath("%md5:maxlength=8%.%ext%", "1bc29b36.jpg");
}
void FilenameTest::testPathOptionMaxDouble()
{
	assertPath("%md5:maxlength=16,maxlength=8%.%ext%", "1bc29b36.jpg");
}
void FilenameTest::testPathOptionDateFormat()
{
	assertPath("%date:format=yyyy-MM-dd%.%ext%", "2016-08-18.jpg");
}
void FilenameTest::testPathOptionTagNamespace()
{
	m_settings->setValue("Save/character_multiple", "keepAll");

	assertPath("%character:includenamespace,unsafe%", "character:character1 character:character2", "", false);
}
void FilenameTest::testPathOptionTagNamespaceSeparator()
{
	m_settings->setValue("Save/separator", "c");
	m_settings->setValue("Save/character_multiple", "keepAll");

	assertPath("%character:includenamespace,unsafe%", "character:character1ccharacter:character2", "", false);
}
void FilenameTest::testPathOptionTagNamespaceComplex()
{
	m_settings->setValue("Save/artist_multiple", "keepAll");
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	m_settings->setValue("Save/character_multiple", "keepAll");
	m_settings->setValue("Save/replaceblanks", true);

	assertPath("%all:ignorenamespace=general,includenamespace,unsafe,separator=\n%\n\n%general%",
			   "artist:artist1\ncharacter:character1\ncharacter:character2\ncopyright:copyright1\ncopyright:copyright2\n\ntag1 tag2 tag3 test_tag1 test_tag2 test_tag3",
			   "", false);
}
void FilenameTest::testPathOptionTagExcludeNamespace()
{
	m_settings->setValue("Save/artist_multiple", "keepAll");
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	m_settings->setValue("Save/character_multiple", "keepAll");
	m_settings->setValue("Save/replaceblanks", true);

	assertPath("%all:includenamespace,excludenamespace=general character,unsafe%",
			   "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist:artist1 character1 character2 copyright:copyright1 copyright:copyright2", "", false);
}
void FilenameTest::testPathOptionTagSeparator()
{
	assertPath("%general:separator=+%", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3");
}
void FilenameTest::testPathOptionTagSeparatorEscape()
{
	assertPath("%general:separator=\\,%", "tag1,tag2,tag3,test_tag1,test_tag2,test_tag3");
}
void FilenameTest::testPathOptionCount()
{
	assertPath("%md5% (%count%).%ext%", "1bc29b36f623ba82aaf6724fd3b16718 (7).jpg");
	assertPath("%md5% (%count:length=3%).%ext%", "1bc29b36f623ba82aaf6724fd3b16718 (007).jpg");
}
void FilenameTest::testPathOptionNumSingle()
{
	assertPath("%id% (%num%).%ext%",
			   "7331 (1).jpg",
			   "tests/resources/tmp/");
}
void FilenameTest::testPathOptionNumSingleLength()
{
	assertPath("%id% (%num:length=3%).%ext%",
			   "7331 (001).jpg",
			   "tests/resources/tmp/");
}
void FilenameTest::testPathOptionNumMultiple()
{
	QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (1).jpg");
	QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (2).jpg");

	assertPath("%id% (%num%).%ext%",
			   "7331 (3).jpg",
			   "tests/resources/tmp/");

	QFile::remove("tests/resources/tmp/7331 (1).jpg");
	QFile::remove("tests/resources/tmp/7331 (2).jpg");
}
void FilenameTest::testPathOptionNumAboveTen()
{
	int count = 15;
	for (int i = 1; i < count; ++i)
		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (" + QString::number(i) + ").jpg");

	assertPath("%id% (%num%).%ext%",
			   "7331 (" + QString::number(count) + ").jpg",
			   "tests/resources/tmp/");

	for (int i = 1; i < count; ++i)
		QFile::remove("tests/resources/tmp/7331 (" + QString::number(i) + ").jpg");
}

void FilenameTest::testPathOptionSort()
{
	m_img->deleteLater();
	m_details["tags_copyright"] = "copyright2 copyright1";
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	m_img = new Image(m_site, m_details, m_profile);

	assertPath("%copyright%", "copyright2 copyright1");
	assertPath("%copyright:sort%", "copyright1 copyright2");
}

void FilenameTest::testPathSpecies()
{
	m_img->deleteLater();
	m_details["tags_species"] = "test_species";
	m_img = new Image(m_site, m_details, m_profile);

	assertPath("%species%.%ext%", "test_species.jpg");
}

void FilenameTest::testPathNoJpeg()
{
	m_img->deleteLater();
	m_details["ext"] = "jpeg";
	m_settings->setValue("Save/noJpeg", true);
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%ext%", "jpg");

	m_img->deleteLater();
	m_details["ext"] = "jpeg";
	m_settings->setValue("Save/noJpeg", false);
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%ext%", "jpeg");
}

void FilenameTest::testPathKeepInvalidTokens()
{
	assertPath("%invalid_token% %ext%", "%invalid_token% jpg", "", true, false, true);
	assertPath("%ext% %invalid_token%", "jpg %invalid_token%", "", true, false, true);
}

void FilenameTest::testPathForbiddenSeparator()
{
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	assertPath("%copyright:separator=/%", "copyright1/copyright2");
}

void FilenameTest::testExpandTokensSimple()
{
	QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

	Filename fn(format);
	m_settings->setValue("Save/character_multiple", "replaceAll");
	QList<QMap<QString, Token>> replaces = fn.expandTokens(format, m_img->tokens(m_profile), m_settings);

	QCOMPARE(replaces.count(), 1);
	QCOMPARE(replaces[0]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[0]["copyright"].toString(), QString("crossover"));
	QCOMPARE(replaces[0]["character"].toString(), QString("group"));
}
void FilenameTest::testExpandTokensMultiple()
{
	QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

	Filename fn(format);
	m_settings->setValue("Save/character_multiple", "multiple");
	QList<QMap<QString, Token>> replaces = fn.expandTokens(format, m_img->tokens(m_profile), m_settings);

	QCOMPARE(replaces.count(), 2);
	QCOMPARE(replaces[0]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[0]["copyright"].toString(), QString("crossover"));
	QCOMPARE(replaces[0]["character"].toString(), QString("character1"));
	QCOMPARE(replaces[1]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[1]["copyright"].toString(), QString("crossover"));
	QCOMPARE(replaces[1]["character"].toString(), QString("character2"));
}
void FilenameTest::testExpandTokensMatrix()
{
	QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

	Filename fn(format);
	m_settings->setValue("Save/character_multiple", "multiple");
	m_settings->setValue("Save/copyright_multiple", "multiple");
	QList<QMap<QString, Token>> replaces = fn.expandTokens(format, m_img->tokens(m_profile), m_settings);

	QCOMPARE(replaces.count(), 4);
	QCOMPARE(replaces[0]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[0]["copyright"].toString(), QString("copyright1"));
	QCOMPARE(replaces[0]["character"].toString(), QString("character1"));
	QCOMPARE(replaces[1]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[1]["copyright"].toString(), QString("copyright1"));
	QCOMPARE(replaces[1]["character"].toString(), QString("character2"));
	QCOMPARE(replaces[2]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[2]["copyright"].toString(), QString("copyright2"));
	QCOMPARE(replaces[2]["character"].toString(), QString("character1"));
	QCOMPARE(replaces[3]["artist"].toString(), QString("artist1"));
	QCOMPARE(replaces[3]["copyright"].toString(), QString("copyright2"));
	QCOMPARE(replaces[3]["character"].toString(), QString("character2"));
}

void FilenameTest::testIsValid()
{
	QCOMPARE(Filename("").isValid(), false);
	QCOMPARE(Filename("%md5%").isValid(), false);
	QCOMPARE(Filename("toto").isValid(), false);
	QCOMPARE(Filename("%toto% %md5%.%ext%").isValid(), false);
	QCOMPARE(Filename("%md5%.%ext%").isValid(), true);
	QCOMPARE(Filename("%id%.%ext%").isValid(), false);
	QCOMPARE(Filename("%website%/%id%.%ext%").isValid(), true);
	QCOMPARE(Filename("%artist%/%copyright%/%character%/%md5%.%ext%").isValid(), true);
	QCOMPARE(Filename("javascript:md5 + '.' + ext;").isValid(), true);
	QCOMPARE(Filename("%md5% %date:format=yyyy-MM-dd%.%ext%").isValid(), true);
	QCOMPARE(Filename("%md5% (%num%).%ext%").isValid(), true);

	QString out;
	Filename("%toto%.%ext%").isValid(m_profile, &out);
	QCOMPARE(out.isEmpty(), false);
}

void FilenameTest::testUseShorterCopyright()
{
	m_details["tags_copyright"] = "test test_2";

	m_img->deleteLater();
	m_settings->setValue("Save/copyright_useshorter", true);
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%copyright%", "test");

	m_img->deleteLater();
	m_settings->setValue("Save/copyright_multiple", "keepAll");
	m_settings->setValue("Save/copyright_useshorter", false);
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%copyright%", "test test_2");

	m_details["tags_copyright"] = "test_2 test";

	m_img->deleteLater();
	m_settings->setValue("Save/copyright_useshorter", true);
	m_img = new Image(m_site, m_details, m_profile);
	assertPath("%copyright%", "test");
}

void FilenameTest::testConditionalsTag()
{
	m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
	m_settings->setValue("Filenames/0_dir", QDir::homePath());
	m_settings->setValue("Filenames/0_cond", "tag4 tag7");
	m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
	m_settings->setValue("Filenames/1_dir", QDir::homePath());
	m_settings->setValue("Filenames/1_cond", "character1");

	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testConditionalsToken()
{
	m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
	m_settings->setValue("Filenames/0_dir", QDir::homePath());
	m_settings->setValue("Filenames/0_cond", "%model%");
	m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
	m_settings->setValue("Filenames/1_dir", QDir::homePath());
	m_settings->setValue("Filenames/1_cond", "%character%");

	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testConditionalsCustom()
{
	m_settings->setValue("Save/Customs/custom1", "tag4 tag7");
	m_settings->setValue("Save/Customs/custom2", "tag1");

	m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
	m_settings->setValue("Filenames/0_dir", QDir::homePath());
	m_settings->setValue("Filenames/0_cond", "%custom1%");
	m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
	m_settings->setValue("Filenames/1_dir", QDir::homePath());
	m_settings->setValue("Filenames/1_cond", "%custom2%");

	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testConditionalsJavascript()
{
	m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
	m_settings->setValue("Filenames/0_dir", QDir::homePath());
	m_settings->setValue("Filenames/0_cond", "javascript:width > 2000");
	m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
	m_settings->setValue("Filenames/1_dir", QDir::homePath());
	m_settings->setValue("Filenames/1_cond", "javascript:width > 400");

	assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testCustoms()
{
	m_settings->setValue("Save/Customs/custom1", "tag1 character1");
	m_settings->setValue("Save/Customs/custom2", "tag3 tag4");

	assertPath("%custom1%", "tag1 character1");
	assertPath("%custom2%", "tag3");
}

void FilenameTest::testReplaceBlanks()
{
	m_settings->setValue("Save/separator", "+");

	m_settings->setValue("Save/replaceblanks", false);
	assertPath("%all%", "tag1+tag2+tag3+test tag1+test tag2+test tag3+artist1+character1+character2+copyright1+copyright2", "", false);
	assertPath("%allo%", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
	assertPath("javascript:all", "tag1+tag2+tag3+test tag1+test tag2+test tag3+artist1+character1+character2+copyright1+copyright2", "", false);
	assertPath("javascript:allo", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);

	m_settings->setValue("Save/replaceblanks", true);
	assertPath("%all%", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3+artist1+character1+character2+copyright1+copyright2", "", false);
	assertPath("%allo%", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
	assertPath("javascript:all", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3+artist1+character1+character2+copyright1+copyright2", "", false);
	assertPath("javascript:allo", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
}

void FilenameTest::testCommand()
{
	 Filename fn("curl -F \"user[name]=User\" -F \"user[password]=1234\" -F \"post[tags]=%all%\" -F \"post[rating]=%rating%\" -F \"post[file]=@%path%\" localhost:9000/post/create");

	 QCOMPARE(fn.path(*m_img, m_profile, "", 0, false, false, false, false),
			  QStringList() << "curl -F \"user[name]=User\" -F \"user[password]=1234\" -F \"post[tags]=tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2\" -F \"post[rating]=safe\" -F \"post[file]=@%path%\" localhost:9000/post/create");
}

void FilenameTest::testFilenameWithMultipleUnderscores()
{
	m_img->deleteLater();

	m_details["file_url"] = "http://test.com/img/__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2.jpg";
	m_details["sample_url"] = "http://test.com/sample/__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2.jpg";
	m_details["preview_url"] = "http://test.com/preview/__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2.jpg";
	m_img = new Image(m_site, m_details, m_profile);

	assertPath("%filename%", "__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2");
}

void FilenameTest::testNeedExactTags()
{
	QCOMPARE(Filename("%md5%.%ext%").needExactTags(false), false);
	QCOMPARE(Filename("%md5%.%ext%").needExactTags(m_site), false);
	QCOMPARE(Filename("javascript:md5 + '.' + ext").needExactTags(false), true);
	QCOMPARE(Filename("%character% %md5%.%ext%").needExactTags(false), true);
	QCOMPARE(Filename("%all:includenamespace% %md5%.%ext%").needExactTags(false), true);

	Filename filename("%filename%.%ext%");
	QCOMPARE(filename.needExactTags(false), false);
	QCOMPARE(filename.needExactTags(true), true);

	Filename date("%date%.%ext%");
	QCOMPARE(date.needExactTags(false, false), false);
	QCOMPARE(date.needExactTags(false, true), true);
}

void FilenameTest::testEscapeMethod()
{
	m_img->deleteLater();
	m_details["md5"] = "good'ol' md5";
	m_img = new Image(m_site, m_details, m_profile);

	Filename fn("INSERT INTO test (%id:escape%, %md5:escape%, %ext:escape%);");
	fn.setEscapeMethod([](const QVariant &val) { return QString("'%1'").arg(val.toString().replace("'", "''")); });

	QCOMPARE(fn.path(*m_img, m_profile).first(), QString("INSERT INTO test ('7331', 'good''ol'' md5', 'jpg');"));
}


void FilenameTest::assertPath(const QString &format, const QString &expected, const QString &path, bool shouldFixFilename, bool fullPath, bool keepInvalidTokens)
{
	assertPath(format, QStringList() << expected, path, shouldFixFilename, fullPath, keepInvalidTokens);
}

void FilenameTest::assertPath(const QString &format, const QStringList &expected, QString path, bool shouldFixFilename, bool fullPath, bool keepInvalidTokens)
{
	if (path.isEmpty())
		path = QDir::homePath();

	// Convert directory separators
	QStringList expectedNative;
	expectedNative.reserve(expected.count());
	for (const QString &exp : expected)
	{
		expectedNative.append(QDir::toNativeSeparators(exp));
	}

	Filename fn(format);
	QStringList actual = fn.path(*m_img, m_profile, path, 7, true, true, shouldFixFilename, fullPath, keepInvalidTokens);
	QCOMPARE(actual, expectedNative);
}


void FilenameTest::assertExpand(const QString &format, const QString &expected)
{
	Filename fn(format);
	QString actual = fn.expandConditionals(format, m_img->tagsString(), m_img->tokens(m_profile), m_settings);
	QCOMPARE(actual, expected);
}

static FilenameTest instance;
