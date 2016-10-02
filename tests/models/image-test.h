#ifndef IMAGE_TEST_H
#define IMAGE_TEST_H

#include "test-suite.h"
#include "models/source.h"
#include "models/site.h"
#include "models/image.h"


class ImageTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testConstructor();
		void testCopy();
		void testHasTag();
		void testHasAnyTag();
		void testHasAllTags();
		void testMd5FromData();
		void testMd5FromFile();
		void testStylishedTags();
		void testUnload();
		void testBlacklisted();
		void testMatchTag();
		void testMatchUnknown();
		void testMatchMathematical();
		void testMatchDate();
		void testMatchRating();
		void testMatchSource();
		void testFilter();
		void testValue();
		void testLoadPreview();
		void testLoadPreviewAbort();
		void testLoadImage();
		void testLoadImageAbort();
		void testLoadDetails();
		void testLoadDetailsAbort();
		void testLoadDetailsImageUrl();
		void testPath();
		void testSave();
#ifdef Q_OS_WIN
		void testSaveError();
#endif
		void testSaveAlreadyExists();
		void testSaveDuplicate();
		void testSaveTextfile();
		void testSaveLog();
		void testSetUrl();

	private:
		Profile *m_profile;
		QSettings *m_settings;
		Source *m_source;
		Site *m_site;
		Image *m_img;
		QMap<QString,QString> m_details;
};

#endif // IMAGE_TEST_H
