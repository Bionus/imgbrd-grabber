#include "functions-test.h"
#include "functions.h"


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
	float size1 = 800;
	QCOMPARE(getUnit(&size1), QString("B"));
	QCOMPARE(size1, 800.0f);

	float size2 = 2048;
	QCOMPARE(getUnit(&size2), QString("KiB"));
	QCOMPARE(size2, 2.0f);

	float size3 = 7340032;
	QCOMPARE(getUnit(&size3), QString("MiB"));
	QCOMPARE(size3, 7.0f);
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


void FunctionsTest::assertFixFilename(int platform, QString filename, QString path, QString expected)
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
