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

		void testDefaultConstructor();
		void testGetFormat();
		void testSetFormat();
		void testPathSimple();
		void testPathComplex();
		void testPathKeepAll();
		void testPathKeepN();
		void testPathKeepNThenAdd();
		void testPathIgnoredTags();
		void testPathEmptyDirs();
		void testPathKeptTokens();
		void testPathFull();
		void testPathSimpleJavascript();
		void testPathInvalidJavascript();
		void testExpandTag();
		void testExpandTokenSimple();
		void testExpandTokenComplex();
		void testPathOptionMax();
		void testPathOptionMaxDouble();
		void testPathOptionDateFormat();
		void testPathOptionTagNamespace();
		void testPathOptionTagNamespaceComplex();
		void testPathOptionTagExcludeNamespace();
		void testPathOptionTagSeparator();
		void testPathOptionCount();
		void testPathOptionNumSingle();
		void testPathOptionNumMultiple();
		void testGetReplacesSimple();
		void testGetReplacesMultiple();
		void testGetReplacesMatrix();
		void testGetReplacesCustom();
		void testIsValid();
		void testUseShorterCopyright();
		void testConditionalsTag();
		void testConditionalsToken();
		void testConditionalsCustom();
		void testCustoms();
		void testReplaceBlanks();
		void testCommand();
		void testNeedExactTagsBasic();
		void testNeedExactTagsSite();
		void testNeedExactTagsJavascript();
		void testNeedExactTagsFilename();
		void testNeedExactTagsToken();

	protected:
		void assertPath(QString format, QString expected, QString path = "", bool shouldFixFilename = true, bool fullPath = false);
		void assertPath(QString format, QStringList expected, QString path = "", bool shouldFixFilename = true, bool fullPath = false);
		void assertExpand(QString format, QString expected);

	private:
		QSettings *m_settings;
		Site *m_site;
		Image *m_img;
		QMap<QString,QString> m_details;
};

#endif // FILENAME_TEST_H
