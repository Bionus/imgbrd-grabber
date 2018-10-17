#ifndef FILENAME_TEST_H
#define FILENAME_TEST_H

#include <QMap>
#include <QString>
#include <QStringList>
#include "test-suite.h"


class Image;
class Profile;
class QSettings;
class Site;
class Source;

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
		void testPathSort();
		void testPathIgnoredTags();
		void testPathEmptyDirs();
		void testPathEmptyDirsNetworkDrive();
		void testPathKeptTokens();
		void testPathFull();
		void testPathSimpleJavascript();
		void testPathJavascriptToken();
		void testPathJavascriptArray();
		void testPathJavascriptDate();
		void testPathInvalidJavascript();
		void testExpandTagSimple();
		void testExpandTagWithInvalidCharacter();
		void testExpandTagInvert();
		void testExpandTagMultiple();
		void testExpandTagIgnore();
		void testExpandTokenSimple();
		void testExpandTokenInvert();
		void testExpandTokenComplex();
		void testExpandMultipleMixed();
		void testExpandEscaping();
		void testPathOptionMax();
		void testPathOptionMaxDouble();
		void testPathOptionDateFormat();
		void testPathOptionTagNamespace();
		void testPathOptionTagNamespaceSeparator();
		void testPathOptionTagNamespaceComplex();
		void testPathOptionTagExcludeNamespace();
		void testPathOptionTagSeparator();
		void testPathOptionTagSeparatorEscape();
		void testPathOptionCount();
		void testPathOptionNumSingle();
		void testPathOptionNumSingleLength();
		void testPathOptionNumMultiple();
		void testPathOptionNumAboveTen();
		void testPathOptionSort();
		void testPathSpecies();
		void testPathMeta();
		void testPathNoJpeg();
		void testPathKeepInvalidTokens();
		void testPathForbiddenSeparator();
		void testExpandTokensSimple();
		void testExpandTokensMultiple();
		void testExpandTokensMatrix();
		void testIsValid();
		void testUseShorterCopyright();
		void testConditionalsTag();
		void testConditionalsMultipleTags();
		void testConditionalsToken();
		void testConditionalsMeta();
		void testConditionalsCustom();
		void testConditionalsJavascript();
		void testCustoms();
		void testReplaceBlanks();
		void testCommand();
		void testFilenameWithMultipleUnderscores();
		void testNeedTemporaryFile();
		void testNeedExactTags();
		void testEscapeMethod();

	protected:
		void assertPath(const QString &format, const QString &expected, const QString &path = "", bool shouldFixFilename = true, bool fullPath = false, bool keepInvalidTokens = false);
		void assertPath(const QString &format, const QStringList &expected, QString path = "", bool shouldFixFilename = true, bool fullPath = false, bool keepInvalidTokens = false);
		void assertExpand(const QString &format, const QString &expected);

	private:
		Profile *m_profile;
		QSettings *m_settings;
		Source *m_source;
		Site *m_site;
		Image *m_img;
		QMap<QString, QString> m_details;
};

#endif // FILENAME_TEST_H
