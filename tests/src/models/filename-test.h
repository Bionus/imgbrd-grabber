#ifndef FILENAME_TEST_H
#define FILENAME_TEST_H

#include "test-suite.h"
#include "models/filename.h"
#include "models/source.h"
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
		void testExpandEscaping();
		void testPathOptionMax();
		void testPathOptionMaxDouble();
		void testPathOptionDateFormat();
		void testPathOptionTagNamespace();
		void testPathOptionTagNamespaceSeparator();
		void testPathOptionTagNamespaceComplex();
		void testPathOptionTagExcludeNamespace();
		void testPathOptionTagSeparator();
		void testPathOptionCount();
		void testPathOptionNumSingle();
		void testPathOptionNumSingleLength();
		void testPathOptionNumMultiple();
		void testPathSpecies();
		void testPathNoJpeg();
		void testPathKeepInvalidTokens();
		void testGetReplacesSimple();
		void testGetReplacesMultiple();
		void testGetReplacesMatrix();
		void testGetReplacesCustom();
		void testGetReplacesSpecies();
		void testGetReplacesSpeciesMultiple();
		void testIsValid();
		void testUseShorterCopyright();
		void testConditionalsTag();
		void testConditionalsToken();
		void testConditionalsCustom();
		void testCustoms();
		void testReplaceBlanks();
		void testCommand();
		void testFilenameWithMultipleUnderscores();
		void testNeedExactTags();
		void testEscapeMethod();

	protected:
		void assertPath(QString format, QString expected, QString path = "", bool shouldFixFilename = true, bool fullPath = false, bool keepInvalidTokens = false);
		void assertPath(QString format, QStringList expected, QString path = "", bool shouldFixFilename = true, bool fullPath = false, bool keepInvalidTokens = false);
		void assertExpand(QString format, QString expected);

	private:
		Profile *m_profile;
		QSettings *m_settings;
		Source *m_source;
		Site *m_site;
		Image *m_img;
		QMap<QString,QString> m_details;
};

#endif // FILENAME_TEST_H
