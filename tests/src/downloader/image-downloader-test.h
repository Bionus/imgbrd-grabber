#ifndef IMAGE_DOWNLOADER_TEST_H
#define IMAGE_DOWNLOADER_TEST_H

#include "downloader/image-downloader.h"
#include <QNetworkAccessManager>
#include "test-suite.h"


class ImageDownloaderTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testSuccessBasic();
		void testSuccessLoadTags();
		void testOpenError();
		void testNotFound();
		void testNetworkError();

	protected:
		Image *createImage();
		void assertDownload(QSharedPointer<Image> img, ImageDownloader *downloader, const QMap<QString, Image::SaveResult> &expected, bool shouldExist);
};

#endif // IMAGE_DOWNLOADER_TEST_H
