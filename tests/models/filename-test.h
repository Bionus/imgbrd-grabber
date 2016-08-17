#ifndef FILENAME_TEST_H
#define FILENAME_TEST_H

#include "test-suite.h"
#include "models/filename.h"
#include "models/site.h"
#include "models/image.h"


class FilenameTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

        void testGetFormat();
        void testPathSimple();

    protected:
        void assertPath(QString format, QString expected, QString path = "C:/test");
        void assertPath(QString format, QStringList expected, QString path = "C:/test");

    private:
        QSettings *m_settings;
        Site *m_site;
        Image *m_img;
};

#endif // FILENAME_TEST_H
