#include <QFileInfo>
#include "functions.h"
#include "functions-test.h"


void FunctionsTest::testFixFilenameWindows()
{
	assertFixFilename(0, "", "C:\\test\\image.jpg", "C:\\test\\image.jpg");
	assertFixFilename(0, "image.jpg", "C:\\test\\", "image.jpg");
	assertFixFilename(0, "image", "C:\\test\\", "image");
	assertFixFilename(0, "folder\\image.jpg", "C:\\test\\", "folder\\image.jpg");
}

void FunctionsTest::testFixFilenameLinux()
{
	assertFixFilename(1, "", "/home/test/image.jpg", "/home/test/image.jpg");
	assertFixFilename(1, "image.jpg", "/home/test/", "image.jpg");
	assertFixFilename(1, "image", "/home/test/", "image");
	assertFixFilename(1, "folder/image.jpg", "/home/test/", "folder/image.jpg");
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
	QCOMPARE(getExtension(""), QString(""));
	QCOMPARE(getExtension("http://test.com/file"), QString(""));
	QCOMPARE(getExtension("http://test.com/file.jpg"), QString("jpg"));
	QCOMPARE(getExtension("http://test.com/file.jpg?toto=1"), QString("jpg"));
}
void FunctionsTest::testSetExtension()
{
	QCOMPARE(setExtension("", "png"), QString(""));
	QCOMPARE(setExtension("http://test.com/file", "png"), QString("http://test.com/file"));
	QCOMPARE(setExtension("http://test.com/file.jpg", "png"), QString("http://test.com/file.png"));
	QCOMPARE(setExtension("http://test.com/file.jpg?toto=1", "png"), QString("http://test.com/file.png?toto=1"));
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
	QCOMPARE(qDateTimeFromString("1492192180").toUTC(),          QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));

	// Standart dates
	QCOMPARE(qDateTimeFromString("2017/04/14 17:49:40").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("2017-04-14 17:49:40").toUTC(), QDateTime(QDate(2017, 4, 14), QTime(17, 49, 40), Qt::UTC));
	QCOMPARE(qDateTimeFromString("2017/04/14 17:49").toUTC(),    QDateTime(QDate(2017, 4, 14), QTime(17, 49), Qt::UTC));
	QCOMPARE(qDateTimeFromString("2017-04-14 17:49").toUTC(),    QDateTime(QDate(2017, 4, 14), QTime(17, 49), Qt::UTC));

	// Danbooru dates
	QCOMPARE(qDateTimeFromString("2017-04-14T17:49:40.498-04:00").toUTC(),  QDateTime(QDate(2017, 4, 14), QTime(17 + 4, 49, 40), Qt::UTC));

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
	QString path = "tests/resources/pages/behoimi.org/results.json";
	QDateTime date = QDateTime::currentDateTimeUtc();

	setFileCreationDate(path, date);

	QDateTime created;
	#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
		created = QFileInfo(path).created();
	#else
		created = QFileInfo(path).birthTime();
	#endif

	QCOMPARE(created.toTime_t(), date.toTime_t());
}
void FunctionsTest::testSetFileCreationDateUtf8()
{
	QString path = "tests/resources/你好.txt";
	QDateTime date = QDateTime::currentDateTimeUtc();

	setFileCreationDate(path, date);

	QDateTime created;
	#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
		created = QFileInfo(path).created();
	#else
		created = QFileInfo(path).birthTime();
	#endif

	QCOMPARE(created.toTime_t(), date.toTime_t());
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

static FunctionsTest instance;
