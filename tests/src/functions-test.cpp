#include <QFileInfo>
#include <QtTest>
#include "functions.h"
#include "functions-test.h"
#include "models/profile.h"


QDateTime fileCreationDate(const QString &path)
{
	QFileInfo fi(path);
	#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
		return fi.created();
	#else
		QDateTime d = fi.birthTime();
		if (d.isValid())
			return d;
		return fi.metadataChangeTime();
	#endif
}


void FunctionsTest::testCopyRecursively()
{
	QString from = QDir::toNativeSeparators("tests/resources/recurse/");
	QString to = QDir::toNativeSeparators("tests/resources/tmp/recurse/");

	QDir(to).removeRecursively();

	QCOMPARE(copyRecursively(from, to), true);
	QCOMPARE(QFile::exists(to + "test.txt"), true);
	QCOMPARE(QFile::exists(to + "test/test1.txt"), true);
	QCOMPARE(QFile::exists(to + "test/test2.txt"), true);
}

void FunctionsTest::testFixFilenameWindows()
{
	assertFixFilename(0, "", "C:\\test\\image.jpg", "C:\\test\\image.jpg");
	assertFixFilename(0, "image.jpg", "C:\\test\\", "image.jpg");
	assertFixFilename(0, "image", "C:\\test\\", "image");
	assertFixFilename(0, "folder\\image.jpg", "C:\\test\\", "folder\\image.jpg");
	assertFixFilename(0, "folder...\\image.jpg", "C:\\test\\", "folder\\image.jpg");
}

void FunctionsTest::testFixFilenameLinux()
{
	assertFixFilename(1, "", "/home/test/image.jpg", "/home/test/image.jpg");
	assertFixFilename(1, "image.jpg", "/home/test/", "image.jpg");
	assertFixFilename(1, "image", "/home/test/", "image");
	assertFixFilename(1, "folder/image.jpg", "/home/test/", "folder/image.jpg");
}

static QByteArray readFile(const QString &path)
{
	QFile f(path);
	if (!f.open(QFile::ReadOnly))
		return QByteArray();

	return f.readAll();
}
void FunctionsTest::testGetExtensionFromHeader()
{
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/gif.gif")), QString("gif"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/png.png")), QString("png"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/jpg.jpg")), QString("jpg"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/bmp.bmp")), QString("bmp"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/webm.webm")), QString("webm"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/mp4.mp4")), QString("mp4"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/swf.swf")), QString("swf"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/ico.ico")), QString("ico"));
	QCOMPARE(getExtensionFromHeader(readFile("tests/resources/minimal/txt.txt")), QString());
}

static QFont makeFont(const QString &name, int size, bool usePixels, int weight, QFont::Style style)
{
	QFont font(name);
	if (usePixels)
		font.setPixelSize(size);
	else
		font.setPointSize(size);
	font.setWeight(weight);
	font.setStyle(style);
	return font;
}
void FunctionsTest::testFontToCss()
{
	QCOMPARE(qFontToCss(makeFont("Arial", 12, false, QFont::Normal, QFont::StyleNormal)), QString("font-family:'Arial'; font-size:12pt; font-style:normal; font-weight:400; text-decoration:none;"));
	QCOMPARE(qFontToCss(makeFont("Arial", 12, true, QFont::Normal, QFont::StyleNormal)), QString("font-family:'Arial'; font-size:12px; font-style:normal; font-weight:400; text-decoration:none;"));
	QCOMPARE(qFontToCss(makeFont("Arial", 12, false, QFont::Bold, QFont::StyleNormal)), QString("font-family:'Arial'; font-size:12pt; font-style:normal; font-weight:600; text-decoration:none;"));
	QCOMPARE(qFontToCss(makeFont("Arial", 12, false, QFont::Normal, QFont::StyleItalic)), QString("font-family:'Arial'; font-size:12pt; font-style:italic; font-weight:400; text-decoration:none;"));
	QCOMPARE(qFontToCss(makeFont("Arial", 12, false, QFont::Normal, QFont::StyleOblique)), QString("font-family:'Arial'; font-size:12pt; font-style:oblique; font-weight:400; text-decoration:none;"));
}

void FunctionsTest::testIsVariantEmpty()
{
	// Int
	QCOMPARE(isVariantEmpty(QVariant(0)), true);
	QCOMPARE(isVariantEmpty(QVariant(1)), false);

	// List
	QCOMPARE(isVariantEmpty(QList<QVariant>()), true);
	QCOMPARE(isVariantEmpty(QList<QVariant>() << 0), false);
	QCOMPARE(isVariantEmpty(QList<QVariant>() << 1), false);

	// Map
	QCOMPARE(isVariantEmpty(QMap<QString, QVariant>()), true);
	QCOMPARE(isVariantEmpty(QMap<QString, QVariant> {{ "", 0 }}), false);
	QCOMPARE(isVariantEmpty(QMap<QString, QVariant> {{ "", 1 }}), false);

	// String
	QCOMPARE(isVariantEmpty(QString()), true);
	QCOMPARE(isVariantEmpty(QString("")), true);
	QCOMPARE(isVariantEmpty(QString("test")), false);

	// String list
	QCOMPARE(isVariantEmpty(QStringList()), true);
	QCOMPARE(isVariantEmpty(QStringList() << ""), false);
	QCOMPARE(isVariantEmpty(QStringList() << "test"), false);

	// Others
	QCOMPARE(isVariantEmpty(QRect(1, 2, 3, 4)), false);
}

void FunctionsTest::testGetUnit()
{
	QStringList units = FILESIZE_UNITS;

	double size1 = 800;
	QCOMPARE(getUnit(&size1), QString(units[0]));
	QCOMPARE(size1, 800.0f);

	double size2 = 2048;
	QCOMPARE(getUnit(&size2), QString(units[1]));
	QCOMPARE(size2, 2.0f);

	double size3 = 7340032;
	QCOMPARE(getUnit(&size3), QString(units[2]));
	QCOMPARE(size3, 7.0f);
}

void FunctionsTest::testFormatFilesize()
{
	QStringList units = FILESIZE_UNITS;

	QCOMPARE(formatFilesize(800), QString("%1 %2").arg("800", units[0]));
	QCOMPARE(formatFilesize(1500), QString("%1 %2").arg("1.46", units[1]));
	QCOMPARE(formatFilesize(2048), QString("%1 %2").arg("2", units[1]));
	QCOMPARE(formatFilesize(5000000), QString("%1 %2").arg("4.77", units[2]));
	QCOMPARE(formatFilesize(7340032), QString("%1 %2").arg("7", units[2]));
}

void FunctionsTest::testGetExtension()
{
	QCOMPARE(getExtension(QUrl("")), QString(""));
	QCOMPARE(getExtension(QUrl("http://test.com/file")), QString(""));
	QCOMPARE(getExtension(QUrl("http://test.com/some.dir/file")), QString(""));
	QCOMPARE(getExtension(QUrl("http://test.com/file.jpg")), QString("jpg"));
	QCOMPARE(getExtension(QUrl("http://test.com/file.jpg?toto=1")), QString("jpg"));
	QCOMPARE(getExtension(QUrl("http://test.com/file.jpg?toto=1")), QString("jpg"));
}
void FunctionsTest::testSetExtension()
{
	QCOMPARE(setExtension(QUrl(""), "png"), QUrl(""));
	QCOMPARE(setExtension(QUrl("http://test.com/file"), "png"), QUrl("http://test.com/file"));
	QCOMPARE(setExtension(QUrl("http://test.com/file.jpg"), "png"), QUrl("http://test.com/file.png"));
	QCOMPARE(setExtension(QUrl("http://test.com/file.jpg?toto=1"), "png"), QUrl("http://test.com/file.png?toto=1"));
}

void FunctionsTest::testLevenshtein()
{
	QCOMPARE(levenshtein("", ""), 0);
	QCOMPARE(levenshtein("1", "1"), 0);
	QCOMPARE(levenshtein("12", "12"), 0);

	QCOMPARE(levenshtein("", "1"), 1);
	QCOMPARE(levenshtein("", "12"), 2);
	QCOMPARE(levenshtein("1", ""), 1);
	QCOMPARE(levenshtein("12", ""), 2);

	QCOMPARE(levenshtein("password", "password1"), 1);
	QCOMPARE(levenshtein("password", "assword"), 1);

	QCOMPARE(levenshtein("password", "Xassword"), 1);
	QCOMPARE(levenshtein("password", "passXord"), 1);

	QCOMPARE(levenshtein("12345678", "23456781"), 2);
	QCOMPARE(levenshtein("12345678", "34567812"), 4);
	QCOMPARE(levenshtein("12345678", "45678123"), 6);
	QCOMPARE(levenshtein("12345678", "56781234"), 8);
	QCOMPARE(levenshtein("12345678", "67812345"), 6);
	QCOMPARE(levenshtein("12345678", "78123456"), 4);
	QCOMPARE(levenshtein("12345678", "81234567"), 2);

	QCOMPARE(levenshtein("123", "321"), 2);
	QCOMPARE(levenshtein("1234", "4321"), 4);
	QCOMPARE(levenshtein("12345", "54321"), 4);
	QCOMPARE(levenshtein("123456", "654321"), 6);
	QCOMPARE(levenshtein("1234567", "7654321"), 6);
	QCOMPARE(levenshtein("12345678", "87654321"), 8);
}

void FunctionsTest::testRemoveWildards()
{
	QCOMPARE(removeWildards(QStringList(), QStringList()), QStringList());
	QCOMPARE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList()), QStringList() << "abc" << "def" << "ghi");
	QCOMPARE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList() << "a*" << "*f"), QStringList() << "ghi");
	QCOMPARE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList() << "no_wildcard"), QStringList() << "abc" << "def" << "ghi");
	QCOMPARE(removeWildards(QStringList() << "abc" << "def" << "ghi", QStringList() << "*not_found*"), QStringList() << "abc" << "def" << "ghi");
}

void FunctionsTest::testDateTimeFromString()
{
	// Timestamps
	QCOMPARE(qDateTimeFromString("1492192180").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));

	// Standart dates
	QCOMPARE(qDateTimeFromString("2017/04/14 17:49:40").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("2017-04-14 17:49:40").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("2017/04/14 17:49").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49), Qt::UTC));
	QCOMPARE(qDateTimeFromString("2017-04-14 17:49").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49), Qt::UTC));

	// Danbooru dates
	QCOMPARE(qDateTimeFromString("2017-04-14T17:49:40.498-04:00").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17 + 4, 49, 40), Qt::UTC));

	// Gelbooru dates
	QCOMPARE(qDateTimeFromString("Tue Apr  4 17:49:40 2017").toUTC(), QDateTime(QDate(2017, 4, 4), QTime(17, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("Fri Apr 14 17:49:40 2017").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("Fri Apr 14 17:49:40 -0500 2017").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17 + 5, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("Fri Apr 14 23:49:40 -0500 2017").toUTC(), QDateTime(QDate(2017, 4, 15), QTime(4, 49, 40), Qt::UTC));
}

void FunctionsTest::testIsUrl()
{
	// Valid URLs
	QCOMPARE(isUrl("http://foo.com/blah_blah"), true);
	QCOMPARE(isUrl("http://foo.com/blah_blah_(wikipedia)"), true);
	QCOMPARE(isUrl("http://foo.com/blah_(wikipedia)_blah#cite-1"), true);
	QCOMPARE(isUrl("http://foo.com/(something)?after=parens"), true);
	QCOMPARE(isUrl("http://1337.net"), true);
	QCOMPARE(isUrl("http://a.b-c.de"), true);
	QCOMPARE(isUrl("http://223.255.255.254"), true);

	// Invalid URLs
	QCOMPARE(isUrl("http://"), false);
	QCOMPARE(isUrl("http://."), false);
	QCOMPARE(isUrl("http://?"), false);
	QCOMPARE(isUrl("//"), false);
	QCOMPARE(isUrl("http:///a"), false);
	QCOMPARE(isUrl("foo.com"), false);
}

void FunctionsTest::testParseMarkdownHeaders()
{
	QCOMPARE(parseMarkdown("# h1"), QString("<h1>h1</h1>"));
	QCOMPARE(parseMarkdown("## h2"), QString("<h2>h2</h2>"));
	QCOMPARE(parseMarkdown("### h3"), QString("<h3>h3</h3>"));
	QCOMPARE(parseMarkdown("#### h4"), QString("<h4>h4</h4>"));
	QCOMPARE(parseMarkdown("##### h5"), QString("<h5>h5</h5>"));
	QCOMPARE(parseMarkdown("###### h6"), QString("<h6>h6</h6>"));
	QCOMPARE(parseMarkdown("####### h7"), QString("<h6>h7</h6>"));
	QCOMPARE(parseMarkdown("a # h1"), QString("a # h1"));
}
void FunctionsTest::testParseMarkdownIssueLinks()
{
	QCOMPARE(parseMarkdown("issue #123"), QString("<a href='" + QString(PROJECT_GITHUB_URL) + "/issues/123'>issue #123</a>"));
	QCOMPARE(parseMarkdown("fix #123"), QString("<a href='" + QString(PROJECT_GITHUB_URL) + "/issues/123'>fix #123</a>"));
	QCOMPARE(parseMarkdown("issue 123"), QString("issue 123"));
}

void FunctionsTest::testSetFileCreationDate()
{
#if !defined(Q_OS_MACOS)
	QString path = "tests/resources/pages/behoimi.org/results.json";
	QDateTime date = QDateTime::currentDateTimeUtc();

	setFileCreationDate(path, date);

	QDateTime created = fileCreationDate(path);
	QCOMPARE(created.toTime_t(), date.toTime_t());
#endif
}
void FunctionsTest::testSetFileCreationDateUtf8()
{
#if !defined(Q_OS_MACOS)
	QString path = "tests/resources/你好.txt";
	QDateTime date = QDateTime::currentDateTimeUtc();

	setFileCreationDate(path, date);

	QDateTime created = fileCreationDate(path);
	QCOMPARE(created.toTime_t(), date.toTime_t());
#endif
}

void FunctionsTest::testGetExternalLogFilesSuffixes()
{
	auto *profile = new Profile("tests/resources/");
	auto *settings = profile->getSettings();

	QCOMPARE(getExternalLogFilesSuffixes(settings), QStringList());

	settings->setValue("LogFiles/0/locationType", 1);
	settings->setValue("LogFiles/0/uniquePath", "path");
	settings->setValue("LogFiles/0/content", "id: %id%");

	QCOMPARE(getExternalLogFilesSuffixes(settings), QStringList());

	settings->setValue("LogFiles/0/locationType", 2);
	settings->setValue("LogFiles/0/suffix", ".xml");

	QCOMPARE(getExternalLogFilesSuffixes(settings), QStringList() << ".xml");

	settings->remove("LogFiles/0/locationType");
	settings->remove("LogFiles/0/suffix");
	settings->remove("LogFiles/0/uniquePath");
	settings->remove("LogFiles/0/content");
}

void FunctionsTest::testFixCloudflareEmail()
{
	QCOMPARE(fixCloudflareEmail("145d505b58595447405146"), QString("IDOLM@STER"));
	QCOMPARE(fixCloudflareEmail("cc9cbea3a6a9afb8e1a5818c9f"), QString("Project-iM@S"));
}
void FunctionsTest::testFixCloudflareEmails()
{
	QCOMPARE(fixCloudflareEmails(R"(<a class="dtext-link dtext-wiki-link" href="/wiki_pages/show_or_new?title=idolm%40ster_cinderella_girls"><span class="__cf_email__" data-cfemail="145d505b58595447405146">[email&#160;protected]</span> Cinderella Girls</a>)"), QString(R"(<a class="dtext-link dtext-wiki-link" href="/wiki_pages/show_or_new?title=idolm%40ster_cinderella_girls">IDOLM@STER Cinderella Girls</a>)"));
	QCOMPARE(fixCloudflareEmails(R"(Koshimizu Sachiko on <span class="__cf_email__" data-cfemail="cc9cbea3a6a9afb8e1a5818c9f">[email&#160;protected]</span>)"), QString("Koshimizu Sachiko on Project-iM@S"));
}

void FunctionsTest::testGetFileMd5()
{
	QCOMPARE(getFileMd5(QString()), QString());
	QCOMPARE(getFileMd5("non_existing_path.txt"), QString());

	QTemporaryFile file;
	QVERIFY(file.open());
	file.write("test");
	file.seek(0);

	QCOMPARE(getFileMd5(file.fileName()), QString("098f6bcd4621d373cade4e832627b4f6")); // md5("test")
}
void FunctionsTest::testGetFilenameMd5()
{
	QCOMPARE(getFilenameMd5("", "%md5%.%ext%"), QString());
	QCOMPARE(getFilenameMd5("lol.jpg", "%md5%.%ext%"), QString());
	QCOMPARE(getFilenameMd5("test/098f6bcd4621d373cade4e832627b4f6.jpg", "%md5%.%ext%"), QString());

	QCOMPARE(getFilenameMd5("098f6bcd4621d373cade4e832627b4f6", "%md5%"), QString("098f6bcd4621d373cade4e832627b4f6"));
	QCOMPARE(getFilenameMd5("098f6bcd4621d373cade4e832627b4f6.jpg", "%md5%.%ext%"), QString("098f6bcd4621d373cade4e832627b4f6"));
	QCOMPARE(getFilenameMd5("test/098f6bcd4621d373cade4e832627b4f6.jpg", "%artist%/%md5%.%ext%"), QString("098f6bcd4621d373cade4e832627b4f6"));
}

void FunctionsTest::testRemoveCacheBuster()
{
	QCOMPARE(removeCacheBuster(QUrl("https://test.com")), QUrl("https://test.com"));
	QCOMPARE(removeCacheBuster(QUrl("https://test.com?string")), QUrl("https://test.com?string"));
	QCOMPARE(removeCacheBuster(QUrl("https://test.com?1234")), QUrl("https://test.com"));
	QCOMPARE(removeCacheBuster(QUrl("https://test.com/path")), QUrl("https://test.com/path"));
	QCOMPARE(removeCacheBuster(QUrl("https://test.com/path?string")), QUrl("https://test.com/path?string"));
	QCOMPARE(removeCacheBuster(QUrl("https://test.com/path?1234")), QUrl("https://test.com/path"));
}


void FunctionsTest::assertFixFilename(int platform, const QString &filename, const QString &path, const QString &expected)
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

	QCOMPARE(actual, expected);
}

QTEST_MAIN(FunctionsTest)
