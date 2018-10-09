#include "language-loader-test.h"
#include <QtTest>
#include "language-loader.h"


void LanguageLoaderTest::testInvalid()
{
	LanguageLoader loader("non_existing_dir/");
	QMap<QString, QString> languages = loader.getAllLanguages();

	QCOMPARE(languages.keys(), QList<QString>() << "");
	QCOMPARE(languages[""], QString("English"));
}

void LanguageLoaderTest::testValid()
{
	LanguageLoader loader("tests/resources/languages/");
	QMap<QString, QString> languages = loader.getAllLanguages();

	QCOMPARE(languages.keys(), QList<QString>() << "English" << "French");
	QCOMPARE(languages["English"], QString("English"));
	QCOMPARE(languages["French"], QString("French - Français"));
}


QTEST_MAIN(LanguageLoaderTest)
