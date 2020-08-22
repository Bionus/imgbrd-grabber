#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QRect>
#include <QSettings>
#include <QTemporaryFile>
#include "functions.h"
#include "models/profile.h"
#include "catch.h"
#include "source-helpers.h"


QDateTime fileCreationDate(const QString &path)
{
	QFileInfo fi(path);
	#ifdef Q_OS_WIN
		#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
			return fi.created();
		#else
			QDateTime d = fi.birthTime();
			if (d.isValid()) {
				return d;
			}
			return fi.metadataChangeTime();
		#endif
	#else
		return fi.lastModified(); // You can't properly change the "creation" time (st_ctime) so we change the mtime on unix
	#endif
}

void assertFixFilename(int platform, const QString &filename, const QString &path, const QString &expected)
{
	QString actual;
	switch (platform)
	{
		case 1: // unix
			actual = fixFilenameLinux(filename, path);
			break;

		default: // windows
			actual = fixFilenameWindows(filename, path);
			break;
	}

	REQUIRE(actual == expected);
}

static QByteArray readFile(const QString &path)
{
	QFile f(path);
	if (!f.open(QFile::ReadOnly)) {
		return QByteArray();
	}

	return f.readAll();
}

static QFont makeFont(const QString &name, int size, bool usePixels, int weight, QFont::Style style)
{
	QFont font(name);
	if (usePixels) {
		font.setPixelSize(size);
	} else {
		font.setPointSize(size);
	}
	font.setWeight(weight);
	font.setStyle(style);
	return font;
}


TEST_CASE("Functions")
{
	SECTION("CopyRecursively")
	{
		QString from = QDir::toNativeSeparators("tests/resources/recurse/");
		QString to = QDir::toNativeSeparators("tests/resources/tmp/recurse/");

		QDir(to).removeRecursively();

		REQUIRE(copyRecursively(from, to));
		REQUIRE(QFile::exists(to + "test.txt"));
		REQUIRE(QFile::exists(to + "test/test1.txt"));
		REQUIRE(QFile::exists(to + "test/test2.txt"));
	}

	SECTION("Fix filename")
	{
		SECTION("Windows")
		{
			assertFixFilename(0, "", "C:\\test\\image.jpg", "C:\\test\\image.jpg");
			assertFixFilename(0, "", "C:\\test", "C:\\test");
			assertFixFilename(0, "", "C:\\test\\", "C:\\test\\");
			assertFixFilename(0, "", " C:\\test\\ ", "C:\\test\\");
			assertFixFilename(0, "image.jpg", "C:\\test\\", "image.jpg");
			assertFixFilename(0, "image", "C:\\test\\", "image");
			assertFixFilename(0, "folder\\image.jpg", "C:\\test\\", "folder\\image.jpg");
			assertFixFilename(0, "folder...\\image.jpg", "C:\\test\\", "folder\\image.jpg");
		}

		SECTION("Linux")
		{
			assertFixFilename(1, "", "/home/test/image.jpg", "/home/test/image.jpg");
			assertFixFilename(1, "", "/home/test", "/home/test");
			assertFixFilename(1, "", "/home/test/", "/home/test/");
			assertFixFilename(1, "", "/home/test/ ", "/home/test/ ");
			assertFixFilename(1, "image.jpg", "/home/test/", "image.jpg");
			assertFixFilename(1, "image", "/home/test/", "image");
			assertFixFilename(1, "folder/image.jpg", "/home/test/", "folder/image.jpg");
		}
	}

	SECTION("GetExtensionFromHeader")
	{
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/gif.gif")) == QString("gif"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/png.png")) == QString("png"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/jpg.jpg")) == QString("jpg"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/bmp.bmp")) == QString("bmp"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/webm.webm")) == QString("webm"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/mp4.mp4")) == QString("mp4"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/swf.swf")) == QString("swf"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/ico.ico")) == QString("ico"));
		REQUIRE(getExtensionFromHeader(readFile("tests/resources/minimal/txt.txt")) == QString());
	}

	SECTION("FontToCss")
	{
		REQUIRE(qFontToCss(makeFont("Arial", 12, false, QFont::Normal, QFont::StyleNormal)) == QString("font-family:'Arial'; font-size:12pt; font-style:normal; font-weight:400; text-decoration:none;"));
		REQUIRE(qFontToCss(makeFont("Arial", 12, true, QFont::Normal, QFont::StyleNormal)) == QString("font-family:'Arial'; font-size:12px; font-style:normal; font-weight:400; text-decoration:none;"));
		REQUIRE(qFontToCss(makeFont("Arial", 12, false, QFont::Bold, QFont::StyleNormal)) == QString("font-family:'Arial'; font-size:12pt; font-style:normal; font-weight:600; text-decoration:none;"));
		REQUIRE(qFontToCss(makeFont("Arial", 12, false, QFont::Normal, QFont::StyleItalic)) == QString("font-family:'Arial'; font-size:12pt; font-style:italic; font-weight:400; text-decoration:none;"));
		REQUIRE(qFontToCss(makeFont("Arial", 12, false, QFont::Normal, QFont::StyleOblique)) == QString("font-family:'Arial'; font-size:12pt; font-style:oblique; font-weight:400; text-decoration:none;"));
	}

	SECTION("IsVariantEmpty")
	{
		// Int
		REQUIRE(isVariantEmpty(QVariant(0)));
		REQUIRE(!isVariantEmpty(QVariant(1)));

		// List
		REQUIRE(isVariantEmpty(QList<QVariant>()));
		REQUIRE(!isVariantEmpty(QList<QVariant>() << 0));
		REQUIRE(!isVariantEmpty(QList<QVariant>() << 1));

		// Map
		REQUIRE(isVariantEmpty(QMap<QString, QVariant>()));
		REQUIRE(!isVariantEmpty(QMap<QString, QVariant> {{ "", 0 }}));
		REQUIRE(!isVariantEmpty(QMap<QString, QVariant> {{ "", 1 }}));

		// String
		REQUIRE(isVariantEmpty(QString()));
		REQUIRE(isVariantEmpty(QString("")));
		REQUIRE(!isVariantEmpty(QString("test")));

		// String list
		REQUIRE(isVariantEmpty(QStringList()));
		REQUIRE(!isVariantEmpty(QStringList() << ""));
		REQUIRE(!isVariantEmpty(QStringList() << "test"));

		// Others
		REQUIRE(!isVariantEmpty(QRect(1, 2, 3, 4)));
	}

	SECTION("GetUnit")
	{
		QStringList units = FILESIZE_UNITS;

		double size1 = 800;
		REQUIRE(getUnit(&size1) == QString(units[0]));
		REQUIRE(size1 == 800.0f);

		double size2 = 2048;
		REQUIRE(getUnit(&size2) == QString(units[1]));
		REQUIRE(size2 == 2.0f);

		double size3 = 7340032;
		REQUIRE(getUnit(&size3) == QString(units[2]));
		REQUIRE(size3 == 7.0f);
	}

	SECTION("FormatFilesize")
	{
		QStringList units = FILESIZE_UNITS;

		REQUIRE(formatFilesize(800) == QString("%1 %2").arg("800", units[0]));
		REQUIRE(formatFilesize(1500) == QString("%1 %2").arg("1.46", units[1]));
		REQUIRE(formatFilesize(2048) == QString("%1 %2").arg("2", units[1]));
		REQUIRE(formatFilesize(5000000) == QString("%1 %2").arg("4.77", units[2]));
		REQUIRE(formatFilesize(7340032) == QString("%1 %2").arg("7", units[2]));
	}

	SECTION("GetExtension")
	{
		REQUIRE(getExtension(QUrl("")) == QString(""));
		REQUIRE(getExtension(QUrl("http://test.com/file")) == QString(""));
		REQUIRE(getExtension(QUrl("http://test.com/some.dir/file")) == QString(""));
		REQUIRE(getExtension(QUrl("http://test.com/file.jpg")) == QString("jpg"));
		REQUIRE(getExtension(QUrl("http://test.com/file.jpg?toto=1")) == QString("jpg"));
		REQUIRE(getExtension(QUrl("http://test.com/file.jpg:large")) == QString("jpg"));
		REQUIRE(getExtension(QUrl("http://test.com/index.php?image=file.jpg")) == QString("jpg"));
	}
	SECTION("SetExtension")
	{
		REQUIRE(setExtension(QUrl(""), "png") == QUrl(""));
		REQUIRE(setExtension(QUrl("http://test.com/file"), "png") == QUrl("http://test.com/file"));
		REQUIRE(setExtension(QUrl("http://test.com/file.jpg"), "png") == QUrl("http://test.com/file.png"));
		REQUIRE(setExtension(QUrl("http://test.com/file.jpg?toto=1"), "png") == QUrl("http://test.com/file.png?toto=1"));
		REQUIRE(setExtension(QUrl("http://test.com/file.jpg:large"), "png") == QUrl("http://test.com/file.png:large"));
	}

	SECTION("Levenshtein")
	{
		REQUIRE(levenshtein("", "") == 0);
		REQUIRE(levenshtein("1", "1") == 0);
		REQUIRE(levenshtein("12", "12") == 0);

		REQUIRE(levenshtein("", "1") == 1);
		REQUIRE(levenshtein("", "12") == 2);
		REQUIRE(levenshtein("1", "") == 1);
		REQUIRE(levenshtein("12", "") == 2);

		REQUIRE(levenshtein("password", "password1") == 1);
		REQUIRE(levenshtein("password", "assword") == 1);

		REQUIRE(levenshtein("password", "Xassword") == 1);
		REQUIRE(levenshtein("password", "passXord") == 1);

		REQUIRE(levenshtein("12345678", "23456781") == 2);
		REQUIRE(levenshtein("12345678", "34567812") == 4);
		REQUIRE(levenshtein("12345678", "45678123") == 6);
		REQUIRE(levenshtein("12345678", "56781234") == 8);
		REQUIRE(levenshtein("12345678", "67812345") == 6);
		REQUIRE(levenshtein("12345678", "78123456") == 4);
		REQUIRE(levenshtein("12345678", "81234567") == 2);

		REQUIRE(levenshtein("123", "321") == 2);
		REQUIRE(levenshtein("1234", "4321") == 4);
		REQUIRE(levenshtein("12345", "54321") == 4);
		REQUIRE(levenshtein("123456", "654321") == 6);
		REQUIRE(levenshtein("1234567", "7654321") == 6);
		REQUIRE(levenshtein("12345678", "87654321") == 8);
	}

	SECTION("RemoveWildards")
	{
		REQUIRE(removeWildards(QStringList(), QStringList()) == QStringList());
		REQUIRE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList()) == QStringList() << "abc" << "def" << "ghi");
		REQUIRE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList() << "a*" << "*f") == QStringList() << "ghi");
		REQUIRE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList() << "no_wildcard") == QStringList() << "abc" << "def" << "ghi");
		REQUIRE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList() << "*not_found*") == QStringList() << "abc" << "def" << "ghi");
	}

	SECTION("DateTimeFromString")
	{
		// Timestamps
		REQUIRE(qDateTimeFromString("1492192180").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));

		// Standart dates
		REQUIRE(qDateTimeFromString("2017/04/14 17:49:40").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
		REQUIRE(qDateTimeFromString("2017-04-14 17:49:40").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
		REQUIRE(qDateTimeFromString("2017/04/14 17:49").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17, 49), Qt::UTC));
		REQUIRE(qDateTimeFromString("2017-04-14 17:49").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17, 49), Qt::UTC));

		// Danbooru dates
		REQUIRE(qDateTimeFromString("2017-04-14T17:49:40.498-04:00").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17 + 4, 49, 40), Qt::UTC));

		// Gelbooru dates
		REQUIRE(qDateTimeFromString("Tue Apr  4 17:49:40 2017").toUTC() == QDateTime(QDate(2017, 4, 4), QTime(17, 49, 40), Qt::UTC));
		REQUIRE(qDateTimeFromString("Fri Apr 14 17:49:40 2017").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
		REQUIRE(qDateTimeFromString("Fri Apr 14 17:49:40 -0500 2017").toUTC() == QDateTime(QDate(2017, 4, 14), QTime(17 + 5, 49, 40), Qt::UTC));
		REQUIRE(qDateTimeFromString("Fri Apr 14 23:49:40 -0500 2017").toUTC() == QDateTime(QDate(2017, 4, 15), QTime(4, 49, 40), Qt::UTC));
	}

	SECTION("IsUrl")
	{
		// Valid URLs
		REQUIRE(isUrl("http://foo.com/blah_blah"));
		REQUIRE(isUrl("http://foo.com/blah_blah_(wikipedia)"));
		REQUIRE(isUrl("http://foo.com/blah_(wikipedia)_blah#cite-1"));
		REQUIRE(isUrl("http://foo.com/(something)?after=parens"));
		REQUIRE(isUrl("http://1337.net"));
		REQUIRE(isUrl("http://a.b-c.de"));
		REQUIRE(isUrl("http://223.255.255.254"));

		// Invalid URLs
		REQUIRE(!isUrl("http://"));
		REQUIRE(!isUrl("http://."));
		REQUIRE(!isUrl("http://?"));
		REQUIRE(!isUrl("//"));
		REQUIRE(!isUrl("http:///a"));
		REQUIRE(!isUrl("foo.com"));
	}

	SECTION("ParseMarkdownHeaders")
	{
		REQUIRE(parseMarkdown("# h1") == QString("<h1>h1</h1>"));
		REQUIRE(parseMarkdown("## h2") == QString("<h2>h2</h2>"));
		REQUIRE(parseMarkdown("### h3") == QString("<h3>h3</h3>"));
		REQUIRE(parseMarkdown("#### h4") == QString("<h4>h4</h4>"));
		REQUIRE(parseMarkdown("##### h5") == QString("<h5>h5</h5>"));
		REQUIRE(parseMarkdown("###### h6") == QString("<h6>h6</h6>"));
		REQUIRE(parseMarkdown("####### h7") == QString("<h6>h7</h6>"));
		REQUIRE(parseMarkdown("a # h1") == QString("a # h1"));
	}
	SECTION("ParseMarkdownIssueLinks")
	{
		REQUIRE(parseMarkdown("issue #123") == QString("<a href='" + QString(PROJECT_GITHUB_URL) + "/issues/123'>issue #123</a>"));
		REQUIRE(parseMarkdown("fix #123") == QString("<a href='" + QString(PROJECT_GITHUB_URL) + "/issues/123'>fix #123</a>"));
		REQUIRE(parseMarkdown("issue 123") == QString("issue 123"));
	}

#ifdef Q_OS_WIN
	SECTION("SetFileCreationDate")
	{
		QString path = "tests/resources/pages/behoimi.org/results.json";
		QDateTime date = QDateTime::currentDateTimeUtc();

		REQUIRE(setFileCreationDate(path, date));

		QDateTime created = fileCreationDate(path);
		REQUIRE(created.toTime_t() == date.toTime_t());
	}
	SECTION("SetFileCreationDateUtf8")
	{
		QString path = "tests/resources/你好.txt";
		QDateTime date = QDateTime::currentDateTimeUtc();

		REQUIRE(setFileCreationDate(path, date));

		QDateTime created = fileCreationDate(path);
		REQUIRE(created.toTime_t() == date.toTime_t());
	}
#endif

	SECTION("GetExternalLogFilesSuffixes")
	{
		auto *profile = makeProfile();
		auto *settings = profile->getSettings();

		REQUIRE(getExternalLogFilesSuffixes(settings) == QStringList());

		settings->setValue("LogFiles/0/locationType", 1);
		settings->setValue("LogFiles/0/uniquePath", "path");
		settings->setValue("LogFiles/0/content", "id: %id%");

		REQUIRE(getExternalLogFilesSuffixes(settings) == QStringList());

		settings->setValue("LogFiles/0/locationType", 2);
		settings->setValue("LogFiles/0/suffix", ".xml");

		REQUIRE(getExternalLogFilesSuffixes(settings) == QStringList() << ".xml");

		settings->remove("LogFiles/0/locationType");
		settings->remove("LogFiles/0/suffix");
		settings->remove("LogFiles/0/uniquePath");
		settings->remove("LogFiles/0/content");

		delete profile;
	}

	SECTION("FixCloudflareEmail")
	{
		REQUIRE(fixCloudflareEmail("145d505b58595447405146") == QString("IDOLM@STER"));
		REQUIRE(fixCloudflareEmail("cc9cbea3a6a9afb8e1a5818c9f") == QString("Project-iM@S"));
	}
	SECTION("FixCloudflareEmails")
	{
		REQUIRE(fixCloudflareEmails(R"(<a class="dtext-link dtext-wiki-link" href="/wiki_pages/show_or_new?title=idolm%40ster_cinderella_girls"><span class="__cf_email__" data-cfemail="145d505b58595447405146">[email&#160;protected]</span> Cinderella Girls</a>)") == QString(R"(<a class="dtext-link dtext-wiki-link" href="/wiki_pages/show_or_new?title=idolm%40ster_cinderella_girls">IDOLM@STER Cinderella Girls</a>)"));
		REQUIRE(fixCloudflareEmails(R"(Koshimizu Sachiko on <span class="__cf_email__" data-cfemail="cc9cbea3a6a9afb8e1a5818c9f">[email&#160;protected]</span>)") == QString("Koshimizu Sachiko on Project-iM@S"));
	}

	SECTION("GetFileMd5")
	{
		REQUIRE(getFileMd5(QString()) == QString());
		REQUIRE(getFileMd5("non_existing_path.txt") == QString());

		QTemporaryFile file;
		REQUIRE(file.open());
		file.write("test");
		file.seek(0);

		REQUIRE(getFileMd5(file.fileName()) == QString("098f6bcd4621d373cade4e832627b4f6")); // md5("test")
	}
	SECTION("GetFilenameMd5")
	{
		REQUIRE(getFilenameMd5("", "%md5%.%ext%") == QString());
		REQUIRE(getFilenameMd5("lol.jpg", "%md5%.%ext%") == QString());
		REQUIRE(getFilenameMd5("test/098f6bcd4621d373cade4e832627b4f6.jpg", "%md5%.%ext%") == QString());

		REQUIRE(getFilenameMd5("098f6bcd4621d373cade4e832627b4f6", "%md5%") == QString("098f6bcd4621d373cade4e832627b4f6"));
		REQUIRE(getFilenameMd5("098f6bcd4621d373cade4e832627b4f6.jpg", "%md5%.%ext%") == QString("098f6bcd4621d373cade4e832627b4f6"));
		REQUIRE(getFilenameMd5("test/098f6bcd4621d373cade4e832627b4f6.jpg", "%artist%/%md5%.%ext%") == QString("098f6bcd4621d373cade4e832627b4f6"));
	}

	SECTION("RemoveCacheBuster")
	{
		REQUIRE(removeCacheBuster(QUrl("https://test.com")) == QUrl("https://test.com"));
		REQUIRE(removeCacheBuster(QUrl("https://test.com?string")) == QUrl("https://test.com?string"));
		REQUIRE(removeCacheBuster(QUrl("https://test.com?1234")) == QUrl("https://test.com"));
		REQUIRE(removeCacheBuster(QUrl("https://test.com/path")) == QUrl("https://test.com/path"));
		REQUIRE(removeCacheBuster(QUrl("https://test.com/path?string")) == QUrl("https://test.com/path?string"));
		REQUIRE(removeCacheBuster(QUrl("https://test.com/path?1234")) == QUrl("https://test.com/path"));
	}
}
