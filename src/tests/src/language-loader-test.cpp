#include <QApplication>
#include "language-loader.h"
#include "catch.h"


TEST_CASE("LanguageLoader")
{
	SECTION("Invalid")
	{
		LanguageLoader loader("non_existing_dir/");
		QMap<QString, QString> languages = loader.getAllLanguages();

		REQUIRE(languages.keys() == QList<QString>() << "");
		REQUIRE(languages[""] == QString("English"));
	}

	SECTION("Valid")
	{
		LanguageLoader loader("tests/resources/languages/");
		QMap<QString, QString> languages = loader.getAllLanguages();

		REQUIRE(languages.keys() == QList<QString>() << "English" << "French");
		REQUIRE(languages["English"] == QString("English"));
		REQUIRE(languages["French"] == QString("French - Français"));
	}

	SECTION("Set language")
	{
		LanguageLoader loader("tests/resources/languages/");

		// The first call should not have any impact because the translators are not installed yet
		REQUIRE(loader.setLanguage("French"));
		REQUIRE(LanguageLoader::tr("Translation test") == QString("Translation test"));

		return; // FIXME

		// Once installed, the translations should immediately be effective
		REQUIRE(loader.install(qApp));
		REQUIRE(LanguageLoader::tr("Translation test") == QString("Test de traduction"));

		// Another call to setLanguage should not require to re-install translators
		REQUIRE(loader.setLanguage("English"));
		REQUIRE(LanguageLoader::tr("Translation test") == QString("Translation test"));

		// Uninstalling the translator should restore the original language
		REQUIRE(loader.setLanguage("French"));
		REQUIRE(loader.uninstall(qApp));
		REQUIRE(LanguageLoader::tr("Translation test") == QString("Translation test"));
	}
}
