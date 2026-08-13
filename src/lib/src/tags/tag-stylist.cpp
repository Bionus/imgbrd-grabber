#include "tags/tag-stylist.h"
#include <QFont>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <algorithm>
#include "functions.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "tags/tag.h"


TagStylist::TagStylist(Profile *profile)
	: m_profile(profile)
{}

QStringList TagStylist::stylished(QList<Tag> tags, bool count, bool noUnderscores, const QString &sort, bool dark) const
{
	// Sort tag list
	if (sort == QLatin1String("type")) {
		std::sort(tags.begin(), tags.end(), sortTagsByType);
	} else if (sort == QLatin1String("name")) {
		std::sort(tags.begin(), tags.end(), sortTagsByName);
	} else if (sort == QLatin1String("count")) {
		std::sort(tags.begin(), tags.end(), sortTagsByCount);
	}

	// Generate style map
	static const QStringList tlist { "artists", "circles", "copyrights", "characters", "species", "metas", "models", "generals", "favorites", "keptForLater", "blacklisteds", "ignoreds" };
	static const QStringList defaults { "#aa0000", "#55bbff", "#aa00aa", "#00aa00", "#ee6600", "#ee6600", "#00aaff", "#000000", "#ffaaff", "#aaaa00", "#444444", "#777777" };
	static const QStringList defaultsDark { "#aa0000", "#55bbff", "#aa00aa", "#00aa00", "#ee6600", "#ee6600", "#00aaff", "#ffffff", "#ffaaff", "#aaaa00", "#444444", "#777777" };
	QMap<QString, QString> styles;
	for (const QString &key : tlist) {
		const QString color = m_profile->getSettings()->value("Coloring/Colors/" + key, (dark ? defaultsDark : defaults).at(tlist.indexOf(key))).toString();
		const QString font = m_profile->getSettings()->value("Coloring/Fonts/" + key).toString();

		QString fontCss;
		static QMap<QString, QString> fontCssCache;
		if (fontCssCache.contains(font)) {
			fontCss = fontCssCache[font];
		} else {
			fontCss = qFontToCss(qFontFromString(font));
		}

		styles.insert(key, "color:" + color + "; " + fontCss);
	}

	QStringList t;
	t.reserve(tags.count());
	for (const Tag &tag : tags) {
		t.append(stylished(tag, styles, count, noUnderscores));
	}

	return t;
}

QString TagStylist::stylished(const Tag &tag, const QMap<QString, QString> &styles, bool count, bool noUnderscores) const
{
	const QString &txt = tag.text();

	// Guess the correct tag family
	const QString plural = tag.type().name() + (!tag.type().name().endsWith('s') ? "s" : "");
	QString key = styles.contains(plural) ? plural : "generals";
	if (m_profile->getBlacklist().contains(txt)) {
		key = "blacklisteds";
	}
	if (m_profile->getIgnored().contains(txt, Qt::CaseInsensitive)) {
		key = "ignoreds";
	}
	for (const QString &t : qAsConst(m_profile->getKeptForLater())) {
		if (t == txt) {
			key = "keptForLater";
		}
	}
	for (const Favorite &fav : qAsConst(m_profile->getFavorites())) {
		if (fav.getName() == txt) {
			key = "favorites";
		}
	}

	QString escaped = txt.toHtmlEscaped();
	QString ret = QString(R"(<a href="%1" style="%2">%3</a>)").arg(QUrl::toPercentEncoding(txt), styles.value(key), noUnderscores ? escaped.replace('_', ' ') : escaped);
	if (count && tag.count() > 0) {
		ret += QString(" <span style=\"color:#aaa\">(%L1)</span>").arg(tag.count());
	}

	return ret;
}
