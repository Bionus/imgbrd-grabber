#include "image-size-test.h"
#include <QtTest>
#include "models/image-size.h"


void ImageSizeTest::testTemporaryPath()
{
	QFile file1("tests/resources/tmp/tmp1.txt");
	QVERIFY(file1.open(QFile::Truncate | QFile::WriteOnly | QFile::Text));
	file1.write("test");
	file1.close();

	QFile file2("tests/resources/tmp/tmp2.txt");
	QVERIFY(file2.open(QFile::Truncate | QFile::WriteOnly | QFile::Text));
	file2.write("test");
	file2.close();

	auto *is = new ImageSize();

	QVERIFY(is->setTemporaryPath(file1.fileName()));
	QVERIFY(!is->setTemporaryPath(file1.fileName()));
	QCOMPARE(is->fileSize, 4);

	QVERIFY(is->setTemporaryPath(file2.fileName()));
	QVERIFY(!file1.exists());

	delete is;
	QVERIFY(!file2.exists());
}

void ImageSizeTest::testSavePath()
{
	QTemporaryFile file;
	QVERIFY(file.open());
	file.write("test");
	file.close();

	ImageSize is;
	QVERIFY(is.setSavePath(file.fileName()));
	QVERIFY(!is.setSavePath(file.fileName()));
	QCOMPARE(is.fileSize, 4);
}

void ImageSizeTest::testSaveDefault()
{
	const QString dest = "tests/resources/tmp/image-size.jpg";

	ImageSize is;
	QCOMPARE(is.save(dest), QString());
	QVERIFY(!QFile::exists(dest));
}

void ImageSizeTest::testSaveMove()
{
	return; // FIXME

	const QString dest = "tests/resources/tmp/image-size.jpg";

	QTemporaryFile file;
	QVERIFY(file.open());
	file.write("test");
	file.close();

	ImageSize is;
	QVERIFY(is.setTemporaryPath(file.fileName()));
	QCOMPARE(is.save(dest), file.fileName());

	QVERIFY(!file.exists());
	QVERIFY(QFile::exists(dest));
	QVERIFY(QFile::remove(dest));
}

void ImageSizeTest::testSaveCopy()
{
	const QString dest = "tests/resources/tmp/image-size.jpg";

	QTemporaryFile file;
	QVERIFY(file.open());
	file.write("test");
	file.close();

	ImageSize is;
	QVERIFY(is.setSavePath(file.fileName()));
	QCOMPARE(is.save(dest), file.fileName());

	QVERIFY(file.exists());
	QVERIFY(QFile::exists(dest));
	QVERIFY(QFile::remove(dest));
}

void ImageSizeTest::testPixmap()
{
	QPixmap pix("tests/resources/image_1x1.png");

	ImageSize is;
	is.setPixmap(pix);

	QCOMPARE(is.pixmap(), pix);
}

void ImageSizeTest::testPixmapRect()
{
	QPixmap pix("tests/resources/image_200x200.png");

	ImageSize is;
	is.rect = QRect(0, 0, 20, 40);
	is.setPixmap(pix);

	QCOMPARE(is.pixmap().size(), QSize(20, 40));
}

void ImageSizeTest::testSerialization()
{
	ImageSize original;
	original.fileSize = 123456;
	original.size = QSize(800, 600);
	original.rect = QRect(10, 20, 30, 40);

	QJsonObject json;
	original.write(json);

	ImageSize dest;
	dest.read(json);

	QCOMPARE(dest.fileSize, original.fileSize);
	QCOMPARE(dest.size, original.size);
	QCOMPARE(dest.rect, original.rect);
}


QTEST_MAIN(ImageSizeTest)
