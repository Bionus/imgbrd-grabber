#ifndef IMAGE_TEST_H
#define IMAGE_TEST_H

#include "test-suite.h"
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
		void testLoadImage();
		void testLoadDetails();
		void testLoadDetailsImageUrl();
		void testPath();
		void testSave();
		void testSaveError();
		void testSaveAlreadyExists();
		void testSaveDuplicate();

    private:
        QSettings *m_settings;
        Site *m_site;
        Image *m_img;
        QMap<QString,QString> m_details;
};

#endif // IMAGE_TEST_H
