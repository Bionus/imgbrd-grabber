#include <QtTest>
#include <QStringList>
#include "e621-test.h"
#include "functions.h"


void E621Test::testSwfUrls()
{
    QList<Image*> images = getImages("Danbooru", "e621.net", "regex", "swf rating:safe");

    // Convert results
    QStringList md5s, urls;
    for (Image *img : images)
    {
        md5s.append(img->md5());
        urls.append(img->url());
    }

    // Check results
    md5s = md5s.mid(0, 3);
    QStringList expected = QStringList() << "cb0523430ab3a75216fe1b3a3a42cac5" << "4533e0a1bf7b132038f7ab3864ecd027" << "d8461800f2a107f2d928fcbca00f6019";
    QCOMPARE(images.count(), 20);
    QCOMPARE(md5s, expected);

    // Compare URLs
    QStringList expectedUrls = QStringList()
                               << "https://static1.e621.net/data/cb/05/cb0523430ab3a75216fe1b3a3a42cac5.swf"
                               << "https://static1.e621.net/data/45/33/4533e0a1bf7b132038f7ab3864ecd027.swf"
                               << "https://static1.e621.net/data/d8/46/d8461800f2a107f2d928fcbca00f6019.swf";
    QStringList actualUrls = urls.mid(0, 3);
    QCOMPARE(actualUrls, expectedUrls);
}


static E621Test instance;
