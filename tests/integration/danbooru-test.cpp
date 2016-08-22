#include <QtTest>
#include <QStringList>
#include "danbooru-test.h"
#include "functions.h"


void DanbooruTest::testHtml()
{
    QList<Image*> images = getImages("Danbooru (2.0)", "danbooru.donmai.us", "regex", "rating:safe");

    // Convert results
    QStringList md5s;
    for (Image *img : images)
    {
        md5s.append(img->md5());
    }

    // Check results
    md5s = md5s.mid(0, 3);
    QStringList expected = QStringList() << "testf2b78f4b9c79e6bef0cdc948fbd495cc" << "a6ce80f18652847857a0258f0046282d" << "91d66f4801086eb474fb5c52af8b0418";
    QCOMPARE(images.count(), 20);
    QCOMPARE(md5s, expected);
}

void DanbooruTest::testXml()
{
    QList<Image*> images = getImages("Danbooru (2.0)", "danbooru.donmai.us", "xml", "rating:safe");

    // Convert results
    QStringList md5s;
    for (Image *img : images)
    {
        md5s.append(img->md5());
    }

    // Check results
    md5s = md5s.mid(0, 3);
    QStringList expected = QStringList() << "test98bf686ca7910ee0ad48e59ba99807d3" << "e5d074c5fe05e1493372e7224d2d198f" << "1e774a4a9d080611fde61e58625e038e";
    QCOMPARE(images.count(), 20);
    QCOMPARE(md5s, expected);
}


static DanbooruTest instance;
