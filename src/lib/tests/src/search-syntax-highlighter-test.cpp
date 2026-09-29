#include <QFont>
#include <QSettings>
#include <QTextDocument>
#include <QTextLayout>
#include "models/favorite.h"
#include "models/profile.h"
#include "search-syntax-highlighter.h"
#include "catch.h"


TEST_CASE("SearchSyntaxHighlighter")
{
	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	const QFont expectedFont("DejaVu Sans Mono", 14, QFont::Bold);
	settings.setValue("Coloring/Fonts/favorites", expectedFont.toString());

	Profile profile(&settings, { Favorite("sonic") });
	QTextDocument document;
	SearchSyntaxHighlighter highlighter(true, &document, &profile);
	document.setPlainText("sonic ");
	highlighter.rehighlight();

	const auto formats = document.begin().layout()->formats();
	REQUIRE(formats.count() == 1);
	REQUIRE(formats[0].format.font().family() == expectedFont.family());
	REQUIRE(formats[0].format.font().pointSize() == expectedFont.pointSize());
	REQUIRE(formats[0].format.font().weight() == expectedFont.weight());
}
