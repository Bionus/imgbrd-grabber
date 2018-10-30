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

void LanguageLoaderTest::testSetLanguage()
{
	LanguageLoader loader("tests/resources/languages/");

	// The first call should not have any impact because the translators are not installed yet
	QVERIFY(loader.setLanguage("French"));
	QCOMPARE(tr("Translation test"), QString("Translation test"));

	// Once installed, the translations should immediately be effective
	QVERIFY(loader.install(qApp));
	QCOMPARE(tr("Translation test"), QString("Test de traduction"));

	// Another call to setLanguage should not require to re-install translators
	QVERIFY(loader.setLanguage("English"));
	QCOMPARE(tr("Translation test"), QString("Translation test"));

	// Uninstalling the translator should restore the original language
	QVERIFY(loader.setLanguage("French"));
	QVERIFY(loader.uninstall(qApp));
	QCOMPARE(tr("Translation test"), QString("Translation test"));
}


QTEST_MAIN(LanguageLoaderTest)
