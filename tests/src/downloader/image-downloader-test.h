#ifndef IMAGE_DOWNLOADER_TEST_H
#define IMAGE_DOWNLOADER_TEST_H

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include "models/image.h"
#include "test-suite.h"


class ImageDownloader;
class Profile;
class Site;
class Source;

class ImageDownloaderTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

		void testSuccessBasic();
		void testSuccessLoadTags();
		void testSuccessLoadTagsExternal();
		void testOpenError();
		void testNotFound();
		void testNetworkError();
		void testOriginalMd5();
		void testGeneratedMd5();
		void testRotateExtension();
		void testBlacklisted();

	protected:
		Image *createImage(bool noMd5 = false);
		void assertDownload(QSharedPointer<Image> img, ImageDownloader *downloader, const QMap<QString, Image::SaveResult> &expected, bool shouldExist, bool onlyCheckValues = false);

	private:
		Profile *m_profile;
		Source *m_source;
		Site *m_site;
};

#endif // IMAGE_DOWNLOADER_TEST_H
