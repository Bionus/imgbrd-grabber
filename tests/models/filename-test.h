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
        void testPathComplex();
        void testPathSimpleJavascript();
        void testExpandTag();
        void testExpandTokenSimple();
        void testExpandTokenComplex();
        void testPathOptionMax();
        void testPathOptionMaxDouble();
        void testPathOptionDateFormat();
        void testPathOptionTagNamespace();
        void testPathOptionTagSeparator();
        void testGetReplacesSimple();
        void testGetReplacesMultiple();
        void testGetReplacesMatrix();
        void testIsValid();
        void testUseShorterCopyright();

    protected:
        void assertPath(QString format, QString expected, QString path = "", bool shouldFixFilename = true);
        void assertPath(QString format, QStringList expected, QString path = "", bool shouldFixFilename = true);
        void assertExpand(QString format, QString expected);

    private:
        QSettings *m_settings;
        Site *m_site;
        Image *m_img;
        QMap<QString,QString> m_details;
};

#endif // FILENAME_TEST_H
