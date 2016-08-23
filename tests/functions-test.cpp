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
    QCOMPARE(getUnit(&size1), QString("o"));
    QCOMPARE(size1, 800.0f);

    float size2 = 2048;
    QCOMPARE(getUnit(&size2), QString("Kio"));
    QCOMPARE(size2, 2.0f);

    float size3 = 7340032;
    QCOMPARE(getUnit(&size3), QString("Mio"));
    QCOMPARE(size3, 7.0f);
}

void FunctionsTest::testGetExtension()
{
    QCOMPARE(getExtension("http://test.com/file"), QString(""));
    QCOMPARE(getExtension("http://test.com/file.jpg"), QString("jpg"));
    QCOMPARE(getExtension("http://test.com/file.jpg?toto=1"), QString("jpg"));
}
void FunctionsTest::testSetExtension()
{
    QCOMPARE(setExtension("http://test.com/file", "png"), QString("http://test.com/file"));
    QCOMPARE(setExtension("http://test.com/file.jpg", "png"), QString("http://test.com/file.png"));
    QCOMPARE(setExtension("http://test.com/file.jpg?toto=1", "png"), QString("http://test.com/file.png?toto=1"));
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
