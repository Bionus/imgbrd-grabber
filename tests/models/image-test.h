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

        void testCopy();
        void testHasTag();
        void testHasAnyTag();
        void testHasAllTags();
        void testUnload();
        void testBlacklisted();
        void testMatchTag();
        void testMatchRating();

    private:
        QSettings *m_settings;
        Site *m_site;
        Image *m_img;
        QMap<QString,QString> m_details;
};

#endif // IMAGE_TEST_H
