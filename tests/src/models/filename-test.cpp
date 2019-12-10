#include <QDir>
#include <QScopedPointer>
#include <QSettings>
#include <QStringList>
#include "loader/token.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"
#include "source-helpers.h"


void assertPath(Profile *profile, Image *img, const QString &format, const QStringList &expected, QString path = "", bool shouldFixFilename = true, bool fullPath = false, bool keepInvalidTokens = false, bool useTokens = true)
{
	if (path.isEmpty()) {
		path = QDir::homePath();
	}

	// Convert directory separators
	QStringList expectedNative;
	if (shouldFixFilename) {
		expectedNative.reserve(expected.count());
		for (const QString &exp : expected) {
			expectedNative.append(QDir::toNativeSeparators(exp));
		}
	} else {
		expectedNative = expected;
	}

	Filename::PathFlags flags = Filename::Complex | Filename::CapLength;
	if (shouldFixFilename) {
		flags |= Filename::Fix;
	}
	if (fullPath) {
		flags |= Filename::IncludeFolder;
	}
	if (keepInvalidTokens) {
		flags |= Filename::KeepInvalidTokens;
	}

	QMap<QString, Token> tokens;
	if (useTokens) {
		tokens = img->tokens(profile);
	} else {
		tokens.insert("allos", img->tokens(profile).value("allos"));
	}

	Filename fn(format);
	QStringList actual = fn.path(useTokens ? img->tokens(profile) : QMap<QString, Token>(), profile, path, 7, flags);
	REQUIRE(actual == expectedNative);
}

void assertPath(Profile *profile, Image *img, const QString &format, const QString &expected, const QString &path = "", bool shouldFixFilename = true, bool fullPath = false, bool keepInvalidTokens = false)
{
	assertPath(profile, img, format, QStringList() << expected, path, shouldFixFilename, fullPath, keepInvalidTokens);
}


TEST_CASE("Filename")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	// Make tmp dir if not already existing
	QDir tmp("tests/resources/");
	if (!tmp.exists("tmp")) {
		tmp.mkdir("tmp");
	}

	QMap<QString, QString> details;
	details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	details["ext"] = "jpg";
	details["author"] = "superauthor";
	details["status"] = "tested";
	details["filename"] = "oldfilename";
	details["search"] = "testing well";
	details["id"] = "7331";
	details["score"] = "21";
	details["parent_id"] = "1337";
	details["file_size"] = "1234567";
	details["creator_id"] = "1234";
	details["has_children"] = "true";
	details["has_note"] = "true";
	details["has_comments"] = "true";
	details["file_url"] = "http://test.com/img/oldfilename.jpg";
	details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	details["width"] = "800";
	details["height"] = "600";
	details["source"] = "http://google.com/";
	details["tags_general"] = "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3";
	details["tags_artist"] = "artist1";
	details["tags_copyright"] = "copyright1 copyright2";
	details["tags_character"] = "character1 character2";
	details["tags_species"] = "";
	details["tags_meta"] = "";
	details["created_at"] = "1471513944";
	details["rating"] = "safe";
	details["name"] = "Test gallery name";

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	auto settings = profile->getSettings();
	settings->setValue("ignoredtags", "");
	settings->setValue("Save/separator", " ");
	settings->setValue("Save/character_value", "group");
	settings->setValue("Save/character_multiple", "replaceAll");
	settings->setValue("Save/copyright_value", "crossover");
	settings->setValue("Save/copyright_multiple", "replaceAll");
	settings->setValue("Save/character_empty", "unknown");
	settings->setValue("Save/replaceblanks", true);

	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	Image *gallery = new Image(site, details, profile);
	details.remove("name");
	Image *img = new Image(site, details, profile);
	img->setParentGallery(QSharedPointer<Image>(gallery));

	SECTION("DefaultConstructor")
	{
		Filename fn;

		REQUIRE(fn.format().isEmpty() == true);
	}

	SECTION("GetFormat")
	{
		QString format = "%md5%.%ext%";
		Filename fn(format);

		REQUIRE(fn.format() == format);
	}

	SECTION("SetFormat")
	{
		QString format = "%md5%.%ext%";
		Filename fn("%id%.%ext%");
		fn.setFormat(format);

		REQUIRE(fn.format() == format);
	}

	SECTION("PathSimple")
	{
		assertPath(profile, img, "%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathComplex")
	{
		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/group/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathKeepAll")
	{
		settings->setValue("Save/character_multiple", "keepAll");

		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/character1 character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathKeepN")
	{
		settings->setValue("Save/character_multiple", "keepN");
		settings->setValue("Save/character_multiple_keepN", 1);

		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/character1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathSort")
	{
		delete img;
		details["tags_copyright"] = "copyright2 copyright1";
		settings->setValue("Save/copyright_multiple", "keepAll");
		settings->setValue("Save/copyright_sort", "name");
		img = new Image(site, details, profile);

		assertPath(profile, img, "%copyright%", "copyright1 copyright2");
	}
	SECTION("PathKeepNThenAdd")
	{
		settings->setValue("Save/character_multiple", "keepNThenAdd");
		settings->setValue("Save/character_multiple_keepNThenAdd_keep", 1);
		settings->setValue("Save/character_multiple_keepNThenAdd_add", " (and %count% of %total%)");
		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/character1 (and 1 of 2)/1bc29b36f623ba82aaf6724fd3b16718.jpg");

		settings->setValue("Save/character_multiple_keepNThenAdd_keep", 2);
		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/character1 character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathIgnoredTags")
	{
		delete img;
		settings->setValue("ignoredtags", "character1");
		img = new Image(site, details, profile);
		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");

		delete img;
		settings->setValue("ignoredtags", "character*");
		img = new Image(site, details, profile);
		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/unknown/1bc29b36f623ba82aaf6724fd3b16718.jpg");

		delete img;
		settings->setValue("Save/character_empty", "");
		img = new Image(site, details, profile);
		assertPath(profile, img,
				"%artist%/%copyright%/%character%/%md5%.%ext%",
				"artist1/crossover/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathEmptyDirs")
	{
		assertPath(profile, img,
				"%artist%/%test%/%md5%.%ext%",
				"artist1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathEmptyDirsNetworkDrive")
	{
		assertPath(profile, img,
				"%md5%.%ext%",
				"//NetworkDrive/Grabber/1bc29b36f623ba82aaf6724fd3b16718.jpg",
				"//NetworkDrive/Grabber", true, true);
	}
	SECTION("PathKeptTokens")
	{
		assertPath(profile, img,
				"%artist%/%path%/%md5%.%ext%",
				"artist1/%path%/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("PathFull")
	{
		assertPath(profile, img,
				"%md5%.%ext%",
				"tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
				"tests/directory/",
				true, true);
		assertPath(profile, img,
				"%md5%.%ext%",
				"tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
				"tests/directory",
				true, true);
		assertPath(profile, img,
				"/%md5%.%ext%",
				"tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
				"tests/directory/",
				true, true);
		assertPath(profile, img,
				"/%md5%.%ext%",
				"tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
				"tests/directory",
				true, true);
	}

	SECTION("PathSimpleJavascript")
	{
		assertPath(profile, img, "javascript:md5 + '.' + ext", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}

	SECTION("PathJavascriptToken")
	{
		settings->setValue("Save/copyright_multiple", "keepAll");
		assertPath(profile, img, "javascript:copyright", "copyright1 copyright2");
	}

	SECTION("PathJavascriptArray")
	{
		// TODO(Bionus): make the "keepAll" unnecessary for this array
		settings->setValue("Save/copyright_multiple", "keepAll");
		assertPath(profile, img, "javascript:copyrights.join('-')", "copyright1-copyright2");
	}

	SECTION("PathJavascriptDate")
	{
		assertPath(profile, img, "javascript:date.toISOString().slice(0, 10)", "2016-08-18");
	}

	SECTION("PathInvalidJavascript")
	{
		assertPath(profile, img, "javascript:'", QStringList());
	}

	SECTION("ExpandTagSimple")
	{
		assertPath(profile, img,
				"<image contains the tag \"tag1\"><\"unknown\" is one of the image tags> %md5%.%ext%",
				"image contains the tag tag1 1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img,
				"<image contains the tag \"unknown\"><\"tag2\" is one of the image tags> %md5%.%ext%",
				"tag2 is one of the image tags 1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img,
				"<image contains the tag \"tag1\"><\"tag2\" is one of the image tags> %md5%.%ext%",
				"image contains the tag tag1tag2 is one of the image tags 1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img,
				"<image contains the tag \"unknown1\"><\"unknown2\" is one of the image tags> %md5%.%ext%",
				"1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandTagWithInvalidCharacter")
	{
		assertPath(profile, img, "<\"fate/stay_night\"/>%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");

		delete img;
		details["tags_copyright"] = "fate/stay_night";
		img = new Image(site, details, profile);

		assertPath(profile, img, "<\"fate/stay_night\"/>%md5%.%ext%", "fate_stay_night/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandTagInvert")
	{
		assertPath(profile, img,
				"<image does not contain the tag !\"tag1\"><!\"unknown\" is not one of the image tags> %md5%.%ext%",
				"unknown is not one of the image tags 1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img,
				"<image does not contain the tag !\"unknown\"><!\"tag2\" is not one of the image tags> %md5%.%ext%",
				"image does not contain the tag unknown 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandTagMultiple")
	{
		assertPath(profile, img, "<\"tag1\" \"tag2\"/>%md5%.%ext%", "tag1 tag2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "<\"tag1\" \"tag4\"/>%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");

		assertPath(profile, img, "<\"tag1\" !\"tag4\"/>%md5%.%ext%", "tag1 tag4/1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "<\"tag1\" !\"tag2\"/>%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandTagIgnore")
	{
		assertPath(profile, img, "<\"tag1\"folder1/>%md5%.%ext%", "tag1folder1/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);
		assertPath(profile, img, "<-\"tag1\"folder1/>%md5%.%ext%", "folder1/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);

		assertPath(profile, img, "<\"tag1\"\"tag2\"folder1/>%md5%.%ext%", "tag1tag2folder1/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);
		assertPath(profile, img, "<-\"tag1\"-\"tag2\"folder1/>%md5%.%ext%", "folder1/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);

		assertPath(profile, img, "<\"tag1\" \"tag2\"/>%md5%.%ext%", "tag1 tag2/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);
		assertPath(profile, img, "<\"tag1\"-\"tag2\"/>%md5%.%ext%", "tag1/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);
		assertPath(profile, img, "<-\"tag1\"\"tag2\"/>%md5%.%ext%", "tag2/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);
		assertPath(profile, img, "<-\"tag1\"-\"tag2\"/>%md5%.%ext%", "/1bc29b36f623ba82aaf6724fd3b16718.jpg", "", false);
	}
	SECTION("ExpandTokenSimple")
	{
		assertPath(profile, img,
				"image - <%artist% some text><text %nothing%> %md5%.%ext%",
				"image - artist1 some text 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandTokenInvert")
	{
		assertPath(profile, img,
				"image - <!%artist% some text><text !%nothing%> %md5%.%ext%",
				"image - text  1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandTokenComplex")
	{
		/*assertPath(profile, img, "image - <%artist% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%",
					"image - artist1 some text  test 1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "image - <%model% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%",
					"image -  1bc29b36f623ba82aaf6724fd3b16718.jpg");*/
	}
	SECTION("ExpandMultipleMixed")
	{
		assertPath(profile, img, "<\"tag1\" %artist%/>%md5%.%ext%", "tag1 artist1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "<\"tag1\" %nothing%/>%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");

		assertPath(profile, img, "<\"tag1\"!%nothing%/>%md5%.%ext%", "tag1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "<\"tag1\"!%artist%/>%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");

		assertPath(profile, img, "<\"tag1\"-%artist%/>%md5%.%ext%", "tag1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "<-\"tag1\"%artist%/>%md5%.%ext%", "artist1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ExpandEscaping")
	{
		assertPath(profile, img, "<<Value>>%md5%<</Value>>", "<Value>1bc29b36f623ba82aaf6724fd3b16718</Value>", "", false);
	}

	SECTION("PathOptionMax")
	{
		assertPath(profile, img, "%md5:maxlength=8%.%ext%", "1bc29b36.jpg");
	}
	SECTION("PathOptionMaxDouble")
	{
		assertPath(profile, img, "%md5:maxlength=16,maxlength=8%.%ext%", "1bc29b36.jpg");
	}
	SECTION("PathOptionDateFormat")
	{
		assertPath(profile, img, "%date:format=yyyy-MM-dd%.%ext%", "2016-08-18.jpg");
	}
	SECTION("PathOptionTagNamespace")
	{
		settings->setValue("Save/character_multiple", "keepAll");

		assertPath(profile, img, "%character:includenamespace,unsafe%", "character:character1 character:character2", "", false);
	}
	SECTION("PathOptionTagNamespaceSeparator")
	{
		settings->setValue("Save/separator", "c");
		settings->setValue("Save/character_multiple", "keepAll");

		assertPath(profile, img, "%character:includenamespace,unsafe%", "character:character1ccharacter:character2", "", false);
	}
	SECTION("PathOptionTagNamespaceComplex")
	{
		settings->setValue("Save/artist_multiple", "keepAll");
		settings->setValue("Save/copyright_multiple", "keepAll");
		settings->setValue("Save/character_multiple", "keepAll");
		settings->setValue("Save/replaceblanks", true);

		assertPath(profile, img,
				"%all:ignorenamespace=general,includenamespace,unsafe,separator=\n%\n\n%general%",
				"artist:artist1\ncharacter:character1\ncharacter:character2\ncopyright:copyright1\ncopyright:copyright2\n\ntag1 tag2 tag3 test_tag1 test_tag2 test_tag3",
				"", false);
	}
	SECTION("PathOptionTagExcludeNamespace")
	{
		settings->setValue("Save/artist_multiple", "keepAll");
		settings->setValue("Save/copyright_multiple", "keepAll");
		settings->setValue("Save/character_multiple", "keepAll");
		settings->setValue("Save/replaceblanks", true);

		assertPath(profile, img,
				"%all:includenamespace,excludenamespace=general character,unsafe%",
				"tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist:artist1 character1 character2 copyright:copyright1 copyright:copyright2", "", false);
	}
	SECTION("PathOptionTagSeparator")
	{
		assertPath(profile, img, "%general:separator=+%", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3");
	}
	SECTION("PathOptionTagSeparatorEscape")
	{
		assertPath(profile, img, "%general:separator=^,%", "tag1,tag2,tag3,test_tag1,test_tag2,test_tag3");
	}
	SECTION("PathOptionCount")
	{
		assertPath(profile, img, "%md5% (%count%).%ext%", "1bc29b36f623ba82aaf6724fd3b16718 (7).jpg");
		assertPath(profile, img, "%md5% (%count:length=3%).%ext%", "1bc29b36f623ba82aaf6724fd3b16718 (007).jpg");
	}
	SECTION("PathOptionNumSingle")
	{
		assertPath(profile, img,
				"%id% (%num%).%ext%",
				"7331 (1).jpg",
				"tests/resources/tmp/");
	}
	SECTION("PathOptionNumSingleLength")
	{
		assertPath(profile, img,
				"%id% (%num:length=3%).%ext%",
				"7331 (001).jpg",
				"tests/resources/tmp/");
	}
	SECTION("PathOptionNumMultiple")
	{
		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (1).jpg");
		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (2).jpg");

		assertPath(profile, img,
				"%id% (%num%).%ext%",
				"7331 (3).jpg",
				"tests/resources/tmp/");

		QFile::remove("tests/resources/tmp/7331 (1).jpg");
		QFile::remove("tests/resources/tmp/7331 (2).jpg");
	}
	SECTION("PathOptionNumNoExt")
	{
		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (1).jpg");
		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (2).png");
		QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (3).png");

		assertPath(profile, img,
				"%id% (%num%).%ext%",
				"7331 (2).jpg",
				"tests/resources/tmp/");

		assertPath(profile, img,
				"%id% (%num:noext%).%ext%",
				"7331 (4).jpg",
				"tests/resources/tmp/");

		QFile::remove("tests/resources/tmp/7331 (1).png");
		QFile::remove("tests/resources/tmp/7331 (2).png");
	}
	SECTION("PathOptionNumAboveTen")
	{
	#if false
		int count = 15;
		for (int i = 1; i < count; ++i) {
			QFile("tests/resources/image_1x1.png").copy("tests/resources/tmp/7331 (" + QString::number(i) + ").jpg");
		}

		assertPath(profile, img,
				"%id% (%num%).%ext%",
				"7331 (" + QString::number(count) + ").jpg",
				"tests/resources/tmp/");

		for (int i = 1; i < count; ++i) {
			QFile::remove("tests/resources/tmp/7331 (" + QString::number(i) + ").jpg");
		}
	#endif
	}

	SECTION("PathOptionSort")
	{
		delete img;
		details["tags_copyright"] = "copyright2 copyright1";
		settings->setValue("Save/copyright_multiple", "keepAll");
		img = new Image(site, details, profile);

		assertPath(profile, img, "%copyright%", "copyright2 copyright1");
		assertPath(profile, img, "%copyright:sort%", "copyright1 copyright2");
	}

	SECTION("PathSpecies")
	{
		delete img;
		details["tags_species"] = "test_species";
		img = new Image(site, details, profile);

		assertPath(profile, img, "%species%.%ext%", "test_species.jpg");
	}

	SECTION("PathMeta")
	{
		delete img;
		details["tags_meta"] = "test_meta";
		img = new Image(site, details, profile);

		assertPath(profile, img, "%meta%.%ext%", "test_meta.jpg");
	}

	SECTION("PathNoJpeg")
	{
		delete img;
		details["ext"] = "jpeg";
		settings->setValue("Save/noJpeg", true);
		img = new Image(site, details, profile);
		assertPath(profile, img, "%ext%", "jpg");

		delete img;
		details["ext"] = "jpeg";
		settings->setValue("Save/noJpeg", false);
		img = new Image(site, details, profile);
		assertPath(profile, img, "%ext%", "jpeg");
	}

	SECTION("PathKeepInvalidTokens")
	{
		assertPath(profile, img, "%invalid_token% %ext%", "%invalid_token% jpg", "", true, false, true);
		assertPath(profile, img, "%ext% %invalid_token%", "jpg %invalid_token%", "", true, false, true);
	}

	SECTION("PathForbiddenSeparator")
	{
		settings->setValue("Save/copyright_multiple", "keepAll");
		assertPath(profile, img, "%copyright:separator=/%", "copyright1/copyright2");
	}

	SECTION("PathGalleryName")
	{
		assertPath(profile, img, "%gallery.name%/%name%-%md5%.%ext%", "Test gallery name/-1bc29b36f623ba82aaf6724fd3b16718.jpg");
		assertPath(profile, img, "javascript:gallery.name + '/' + name + '-' + md5 + '.' + ext", "Test gallery name/-1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}

	SECTION("ExpandTokensSimple")
	{
		QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

		Filename fn(format);
		settings->setValue("Save/character_multiple", "replaceAll");
		QList<QMap<QString, Token>> replaces = fn.expandTokens(img->tokens(profile), settings);

		REQUIRE(replaces.count() == 1);
		REQUIRE(replaces[0]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[0]["copyright"].toString() == QString("crossover"));
		REQUIRE(replaces[0]["character"].toString() == QString("group"));
	}
	SECTION("ExpandTokensMultiple")
	{
		QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

		Filename fn(format);
		settings->setValue("Save/character_multiple", "multiple");
		QList<QMap<QString, Token>> replaces = fn.expandTokens(img->tokens(profile), settings);

		REQUIRE(replaces.count() == 2);
		REQUIRE(replaces[0]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[0]["copyright"].toString() == QString("crossover"));
		REQUIRE(replaces[0]["character"].toString() == QString("character1"));
		REQUIRE(replaces[1]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[1]["copyright"].toString() == QString("crossover"));
		REQUIRE(replaces[1]["character"].toString() == QString("character2"));
	}
	SECTION("ExpandTokensMatrix")
	{
		QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

		Filename fn(format);
		settings->setValue("Save/character_multiple", "multiple");
		settings->setValue("Save/copyright_multiple", "multiple");
		QList<QMap<QString, Token>> replaces = fn.expandTokens(img->tokens(profile), settings);

		REQUIRE(replaces.count() == 4);
		REQUIRE(replaces[0]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[0]["copyright"].toString() == QString("copyright1"));
		REQUIRE(replaces[0]["character"].toString() == QString("character1"));
		REQUIRE(replaces[1]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[1]["copyright"].toString() == QString("copyright1"));
		REQUIRE(replaces[1]["character"].toString() == QString("character2"));
		REQUIRE(replaces[2]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[2]["copyright"].toString() == QString("copyright2"));
		REQUIRE(replaces[2]["character"].toString() == QString("character1"));
		REQUIRE(replaces[3]["artist"].toString() == QString("artist1"));
		REQUIRE(replaces[3]["copyright"].toString() == QString("copyright2"));
		REQUIRE(replaces[3]["character"].toString() == QString("character2"));
	}

	SECTION("IsValid")
	{
		REQUIRE(Filename("").isValid() == false);
		REQUIRE(Filename("%md5%").isValid() == false);
		REQUIRE(Filename("toto").isValid() == false);
		REQUIRE(Filename("%toto% %md5%.%ext%").isValid() == false);
		REQUIRE(Filename("%md5%.%ext%").isValid() == true);
		REQUIRE(Filename("%id%.%ext%").isValid() == false);
		REQUIRE(Filename("%website%/%id%.%ext%").isValid() == true);
		REQUIRE(Filename("%artist%/%copyright%/%character%/%md5%.%ext%").isValid() == true);
		REQUIRE(Filename("javascript:md5 + '.' + ext;").isValid() == true);
		REQUIRE(Filename("%md5% %date:format=yyyy-MM-dd%.%ext%").isValid() == true);
		REQUIRE(Filename("%md5% (%num%).%ext%").isValid() == true);

		REQUIRE(Filename("%gallery.id%/%md5%.%ext%").isValid() == true);
		REQUIRE(Filename("%toto.id%/%md5%.%ext%").isValid() == false);

		QString out;
		Filename("%toto%.%ext%").isValid(profile, &out);
		REQUIRE(out.isEmpty() == false);
	}

	SECTION("UseShorterCopyright")
	{
		details["tags_copyright"] = "test test_2";

		delete img;
		settings->setValue("Save/copyright_useshorter", true);
		img = new Image(site, details, profile);
		assertPath(profile, img, "%copyright%", "test");

		delete img;
		settings->setValue("Save/copyright_multiple", "keepAll");
		settings->setValue("Save/copyright_useshorter", false);
		img = new Image(site, details, profile);
		assertPath(profile, img, "%copyright%", "test test_2");

		details["tags_copyright"] = "test_2 test";

		delete img;
		settings->setValue("Save/copyright_useshorter", true);
		img = new Image(site, details, profile);
		assertPath(profile, img, "%copyright%", "test");
	}

	SECTION("ConditionalsTag")
	{
		settings->setValue("Filenames/0_fn", "%md5%.%ext%");
		settings->setValue("Filenames/0_dir", QDir::homePath());
		settings->setValue("Filenames/0_cond", "tag7");
		settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/1_dir", QDir::homePath());
		settings->setValue("Filenames/1_cond", "");
		settings->setValue("Filenames/2_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/2_dir", QDir::homePath());
		settings->setValue("Filenames/2_cond", "character1");

		assertPath(profile, img, "%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ConditionalsMultipleTags")
	{
		settings->setValue("Filenames/0_fn", "%md5%.%ext%");
		settings->setValue("Filenames/0_dir", QDir::homePath());
		settings->setValue("Filenames/0_cond", "tag7");
		settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/1_dir", QDir::homePath());
		settings->setValue("Filenames/1_cond", "tag1 tag8");

		assertPath(profile, img, "%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ConditionalsToken")
	{
		settings->setValue("Filenames/0_fn", "%md5%.%ext%");
		settings->setValue("Filenames/0_dir", QDir::homePath());
		settings->setValue("Filenames/0_cond", "%model%");
		settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/1_dir", QDir::homePath());
		settings->setValue("Filenames/1_cond", "%character%");

		assertPath(profile, img, "%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ConditionalsMeta")
	{
		settings->setValue("Filenames/0_fn", "explicit %md5%.%ext%");
		settings->setValue("Filenames/0_dir", QDir::homePath());
		settings->setValue("Filenames/0_cond", "rating:explicit");
		settings->setValue("Filenames/1_fn", "safe %md5%.%ext%");
		settings->setValue("Filenames/1_dir", QDir::homePath());
		settings->setValue("Filenames/1_cond", "rating:safe");

		assertPath(profile, img, "%artist%/%copyright%/%character%/%md5%.%ext%", "safe 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ConditionalsCustom")
	{
		settings->setValue("Save/Customs/custom1", "tag4 tag7");
		settings->setValue("Save/Customs/custom2", "tag1");

		settings->setValue("Filenames/0_fn", "%md5%.%ext%");
		settings->setValue("Filenames/0_dir", QDir::homePath());
		settings->setValue("Filenames/0_cond", "%custom1%");
		settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/1_dir", QDir::homePath());
		settings->setValue("Filenames/1_cond", "%custom2%");

		assertPath(profile, img, "%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}
	SECTION("ConditionalsJavascript")
	{
		settings->setValue("Filenames/0_fn", "%md5%.%ext%");
		settings->setValue("Filenames/0_dir", QDir::homePath());
		settings->setValue("Filenames/0_cond", "javascript:width > 2000");
		settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/1_dir", QDir::homePath());
		settings->setValue("Filenames/1_cond", "javascript:'");
		settings->setValue("Filenames/2_fn", "%id% %md5%.%ext%");
		settings->setValue("Filenames/2_dir", QDir::homePath());
		settings->setValue("Filenames/2_cond", "javascript:width > 400");

		assertPath(profile, img, "%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
	}

	SECTION("Customs")
	{
		settings->setValue("Save/Customs/custom1", "tag1 character1");
		settings->setValue("Save/Customs/custom2", "tag3 tag4");

		assertPath(profile, img, "%custom1%", "tag1 character1");
		assertPath(profile, img, "%custom2%", "tag3");
	}

	SECTION("ReplaceBlanks")
	{
		settings->setValue("Save/separator", "+");

		settings->setValue("Save/replaceblanks", false);
		assertPath(profile, img, "%all%", "tag1+tag2+tag3+test tag1+test tag2+test tag3+artist1+character1+character2+copyright1+copyright2", "", false);
		assertPath(profile, img, "%allo%", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
		assertPath(profile, img, "javascript:all", "tag1+tag2+tag3+test tag1+test tag2+test tag3+artist1+character1+character2+copyright1+copyright2", "", false);
		assertPath(profile, img, "javascript:allo", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);

		settings->setValue("Save/replaceblanks", true);
		assertPath(profile, img, "%all%", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3+artist1+character1+character2+copyright1+copyright2", "", false);
		assertPath(profile, img, "%allo%", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
		assertPath(profile, img, "javascript:all", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3+artist1+character1+character2+copyright1+copyright2", "", false);
		assertPath(profile, img, "javascript:allo", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
	}

	SECTION("Command")
	{
		Filename fn("curl -F \"user[name]=User\" -F \"user[password]=1234\" -F \"post[tags]=%all%\" -F \"post[rating]=%rating%\" -F \"post[file]=@%path%\" localhost:9000/post/create");

		REQUIRE(fn.path(*img, profile, "", 0, Filename::None) == QStringList() << "curl -F \"user[name]=User\" -F \"user[password]=1234\" -F \"post[tags]=tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2\" -F \"post[rating]=safe\" -F \"post[file]=@%path%\" localhost:9000/post/create");
	}

	SECTION("FilenameWithMultipleUnderscores")
	{
		delete img;

		details["file_url"] = "http://test.com/img/__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2.jpg";
		details["sample_url"] = "http://test.com/sample/__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2.jpg";
		details["preview_url"] = "http://test.com/preview/__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2.jpg";
		img = new Image(site, details, profile);

		assertPath(profile, img, "%filename%", "__fubuki_kantai_collection_drawn_by_minosu__23d36b216c1a3f4e219c4642e221e1a2");
	}

	SECTION("NeedTemporaryFile")
	{
		QMap<QString, Token> tokens;

		tokens =
		{
			{ "md5", Token("", "") },
			{ "filesize", Token(0, 0) },
			{ "id", Token(0, 0) },
		};
		REQUIRE(Filename("%md5%.%ext%").needTemporaryFile(tokens));
		REQUIRE(Filename("%id% (%filesize%).%ext%").needTemporaryFile(tokens));
		REQUIRE(!Filename("%id%.%ext%").needTemporaryFile(tokens));

		tokens =
		{
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718", "") },
			{ "filesize", Token(123, 0) },
			{ "id", Token(456, 0) },
		};
		REQUIRE(!Filename("%md5%.%ext%").needTemporaryFile(tokens));
		REQUIRE(!Filename("%id% (%filesize%).%ext%").needTemporaryFile(tokens));
		REQUIRE(!Filename("%id%.%ext%").needTemporaryFile(tokens));
	}

	SECTION("NeedExactTags")
	{
		REQUIRE(Filename("%md5%.%ext%").needExactTags(nullptr) == 0);
		REQUIRE(Filename("%md5%.%ext%").needExactTags(site) == 0);
		REQUIRE(Filename("javascript:md5 + '.' + ext").needExactTags(nullptr) == 2);
		REQUIRE(Filename("%character% %md5%.%ext%").needExactTags(nullptr) == 1);
		REQUIRE(Filename("%all:includenamespace% %md5%.%ext%").needExactTags(nullptr) == 1);

		Filename filename("%filename%.%ext%");
		REQUIRE(filename.needExactTags() == 0);
		REQUIRE(filename.needExactTags(QStringList() << "filename") == 2);

		Filename date("%date%.%ext%");
		REQUIRE(date.needExactTags() == 0);
		REQUIRE(date.needExactTags(QStringList() << "date") == 2);
	}

	SECTION("EscapeMethod")
	{
		delete img;
		details["md5"] = "good'ol' md5";
		img = new Image(site, details, profile);

		Filename fn("INSERT INTO test (%id:escape%, %md5:escape%, %ext:escape%);");
		fn.setEscapeMethod([](const QVariant &val) { return QString("'%1'").arg(val.toString().replace("'", "''")); });

		REQUIRE(fn.path(*img, profile).first() == QString("INSERT INTO test ('7331', 'good''ol'' md5', 'jpg');"));
	}
}
