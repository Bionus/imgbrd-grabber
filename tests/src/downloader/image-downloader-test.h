#ifndef IMAGE_DOWNLOADER_TEST_H
#define IMAGE_DOWNLOADER_TEST_H

#include "downloader/image-downloader.h"
#include <QNetworkAccessManager>
#include "test-suite.h"


class ImageDownloaderTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void cleanup();

		void testSuccessBasic();
		void testSuccessLoadTags();
		void testOpenError();
		void testNotFound();
		void testNetworkError();
		void testOriginalMd5();
		void testGeneratedMd5();
		void testRotateExtension();

	protected:
		Image *createImage(bool noMd5 = false);
		void assertDownload(QSharedPointer<Image> img, ImageDownloader *downloader, const QMap<QString, Image::SaveResult> &expected, bool shouldExist);
};

#endif // IMAGE_DOWNLOADER_TEST_H
