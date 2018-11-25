#include "image-save-result-test.h"
#include <QtTest>
#include "downloader/image-save-result.h"


void ImageSaveResultTest::testCompare()
{
	ImageSaveResult a;
	a.path = "path";
	a.size = Image::Size::Full;
	a.result = Image::SaveResult::Saved;

	ImageSaveResult b;
	b.path = "path";
	b.size = Image::Size::Full;
	b.result = Image::SaveResult::Saved;

	ImageSaveResult c;
	c.path = "sample";
	c.size = Image::Size::Sample;
	c.result = Image::SaveResult::Saved;

	QVERIFY(a == b);
	QVERIFY(b == a);
	QVERIFY(a != c);
	QVERIFY(b != c);
	QVERIFY(c == c);
}


QTEST_MAIN(ImageSaveResultTest)
