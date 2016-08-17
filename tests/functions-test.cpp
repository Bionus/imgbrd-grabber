#include <QtTest>
#include "functions-test.h"
#include "functions.h"


void FunctionsTest::testFixFilenameWindows()
{
    assertFixFilename(0, "", "C:\\test\\image.jpg", "C:\\test\\image.jpg");
    assertFixFilename(0, "image.jpg", "C:\\test\\", "image.jpg");
}

void FunctionsTest::testFixFilenameLinux()
{
    assertFixFilename(1, "", "/home/test/image.jpg", "/home/test/image.jpg");
    assertFixFilename(1, "image.jpg", "/home/test/", "image.jpg");
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
