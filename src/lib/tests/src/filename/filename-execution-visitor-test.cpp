#include <QSettings>
#include <QString>
#include "filename/filename-parser.h"
#include "filename/visitors/filename-execution-visitor.h"
#include "loader/token.h"
#include "models/profile.h"
#include "catch.h"


static QString executeFilename(const QString &filename, const QMap<QString, Token> &tokens)
{
	FilenameParser parser(filename);
	auto ast = parser.parseRoot();

	REQUIRE(parser.error() == QString());
	REQUIRE(ast != nullptr);

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	FilenameExecutionVisitor executionVisitor(tokens, &settings);
	return executionVisitor.run(*ast);
}

TEST_CASE("FilenameExecutionVisitor")
{
	SECTION("Empty")
	{
		const QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		const QString result = executeFilename("", tokens);

		REQUIRE(result == QString());
	}

	SECTION("Basic")
	{
		const QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		const QString result = executeFilename("image.jpg", tokens);

		REQUIRE(result == QString("image.jpg"));
	}

	SECTION("Token")
	{
		const QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		const QString result = executeFilename("out/%md5%.%ext%", tokens);

		REQUIRE(result == QString("out/1bc29b36f623ba82aaf6724fd3b16718.jpg"));
	}

	SECTION("Token list count")
	{
		const QMap<QString, Token> tokens {{ "list", Token(QStringList() << "a" << "b" << "c") }};

		const QString result = executeFilename("%list:count%", tokens);

		REQUIRE(result == QString("3"));
	}

	SECTION("Sub-object conditional")
	{
		SECTION("The token exists")
		{
			const QMap<QString, Token> gallery {{ "name", Token("some gallery") }};
			const QMap<QString, Token> tokens {
				{ "gallery", Token(QVariant::fromValue(gallery)) },
				{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
				{ "ext", Token("jpg") }
			};

			const QString result = executeFilename("<galleries/%gallery.name%/>%md5%.%ext%", tokens);

			REQUIRE(result == QString("galleries/some gallery/1bc29b36f623ba82aaf6724fd3b16718.jpg"));
		}

		SECTION("Missing token")
		{
			const QMap<QString, Token> gallery {{ "name", Token("some gallery") }};
			const QMap<QString, Token> tokens {
				{ "gallery", Token(QVariant::fromValue(gallery)) },
				{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
				{ "ext", Token("jpg") }
			};

			const QString result = executeFilename("<galleries/%gallery.id%/>%md5%.%ext%", tokens);

			REQUIRE(result == QString("1bc29b36f623ba82aaf6724fd3b16718.jpg"));
		}
	}

	SECTION("Missing parent")
	{
		const QMap<QString, Token> tokens {
			{ "md5", Token("1bc29b36f623ba82aaf6724fd3b16718") },
			{ "ext", Token("jpg") }
		};

		const QString result = executeFilename("%gallery.id%/%md5%.%ext%", tokens);

		REQUIRE(result == QString("/1bc29b36f623ba82aaf6724fd3b16718.jpg"));
	}

	SECTION("Score length")
	{
		REQUIRE(executeFilename("%score:length=5%", {{ "score", Token("123") }}) == QString("00123"));
		REQUIRE(executeFilename("%score:length=5%", {{ "score", Token("12.3") }}) == QString("00012.3"));
		REQUIRE(executeFilename("%score:length=5%", {{ "score", Token("0.123") }}) == QString("00000.123"));
		REQUIRE(executeFilename("%score:length=5%", {{ "score", Token(".123") }}) == QString("00000.123"));
	}
}
