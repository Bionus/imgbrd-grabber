#include <QSettings>
#include <QString>
#include "filename/filename-execution-visitor.h"
#include "filename/filename-parser.h"
#include "loader/token.h"
#include "models/profile.h"
#include "catch.h"


TEST_CASE("FilenameExecutionVisitor")
{
	SECTION("Empty")
	{
		QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		FilenameParser parser("");
		auto ast = parser.parseRoot();

		REQUIRE(parser.error() == QString());
		REQUIRE(ast != nullptr);

		QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
		FilenameExecutionVisitor executionVisitor(tokens, &settings);
		QString result = executionVisitor.run(*ast);

		REQUIRE(result == QString());
	}

	SECTION("Basic")
	{
		QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		FilenameParser parser("image.jpg");
		auto ast = parser.parseRoot();

		REQUIRE(parser.error() == QString());
		REQUIRE(ast != nullptr);

		QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
		FilenameExecutionVisitor executionVisitor(tokens, &settings);
		QString result = executionVisitor.run(*ast);

		REQUIRE(result == QString("image.jpg"));
	}

	SECTION("Token")
	{
		QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		FilenameParser parser("out/%md5%.%ext%");
		auto ast = parser.parseRoot();

		REQUIRE(parser.error() == QString());
		REQUIRE(ast != nullptr);

		QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
		FilenameExecutionVisitor executionVisitor(tokens, &settings);
		QString result = executionVisitor.run(*ast);

		REQUIRE(result == QString("out/1bc29b36f623ba82aaf6724fd3b16718.jpg"));
	}

	SECTION("Token list count")
	{
		QMap<QString, Token> tokens {{ "list", Token(QStringList() << "a" << "b" << "c") }};

		FilenameParser parser("%list:count%");
		auto ast = parser.parseRoot();

		REQUIRE(parser.error() == QString());
		REQUIRE(ast != nullptr);

		QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
		FilenameExecutionVisitor executionVisitor(tokens, &settings);
		QString result = executionVisitor.run(*ast);

		REQUIRE(result == QString("3"));
	}

	SECTION("Sub-object conditional")
	{
		SECTION("The token exists")
		{
			QMap<QString, Token> gallery {{ "name", Token("some gallery") }};
			QMap<QString, Token> tokens {
				{ "gallery", Token(QVariant::fromValue(gallery)) },
				{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
				{ "ext", Token("jpg") }
			};

			FilenameParser parser("<galleries/%gallery.name%/>%md5%.%ext%");
			auto ast = parser.parseRoot();

			REQUIRE(parser.error() == QString());
			REQUIRE(ast != nullptr);

			QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
			FilenameExecutionVisitor executionVisitor(tokens, &settings);
			QString result = executionVisitor.run(*ast);

			REQUIRE(result == QString("galleries/some gallery/1bc29b36f623ba82aaf6724fd3b16718.jpg"));
		}

		SECTION("Missing token")
		{
			QMap<QString, Token> gallery {{ "name", Token("some gallery") }};
			QMap<QString, Token> tokens {
				{ "gallery", Token(QVariant::fromValue(gallery)) },
				{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
				{ "ext", Token("jpg") }
			};

			FilenameParser parser("<galleries/%gallery.id%/>%md5%.%ext%");
			auto ast = parser.parseRoot();

			REQUIRE(parser.error() == QString());
			REQUIRE(ast != nullptr);

			QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
			FilenameExecutionVisitor executionVisitor(tokens, &settings);
			QString result = executionVisitor.run(*ast);

			REQUIRE(result == QString("1bc29b36f623ba82aaf6724fd3b16718.jpg"));
		}
	}
}
