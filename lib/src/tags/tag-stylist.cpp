#include "tags/tag-stylist.h"
#include <algorithm>
#include "functions.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "tags/tag.h"


TagStylist::TagStylist(Profile *profile)
	: m_profile(profile)
{}

QStringList TagStylist::stylished(QList<Tag> tags, bool count, bool noUnderscores, const QString &sort) const
{
	// Sort tag list
	if (sort == QLatin1String("type"))
		std::sort(tags.begin(), tags.end(), sortTagsByType);
	else if (sort == QLatin1String("name"))
		std::sort(tags.begin(), tags.end(), sortTagsByName);
	else if (sort == QLatin1String("count"))
		std::sort(tags.begin(), tags.end(), sortTagsByCount);

	// Generate style map
	static const QStringList tlist = QStringList() << "artists" << "circles" << "copyrights" << "characters" << "species" << "metas" << "models" << "generals" << "favorites" << "keptForLater" << "blacklisteds" << "ignoreds" << "favorites";
	static const QStringList defaults = QStringList() << "#aa0000" << "#55bbff" << "#aa00aa" << "#00aa00" << "#ee6600" << "#ee6600" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000" << "#000000" << "#999999" << "#ffcccc";
	QMap<QString, QString> styles;
	for (const QString &key : tlist)
	{
		QFont font;
		font.fromString(m_profile->getSettings()->value("Coloring/Fonts/" + key).toString());
		const QString color = m_profile->getSettings()->value("Coloring/Colors/" + key, defaults.at(tlist.indexOf(key))).toString();
		styles.insert(key, "color:" + color + "; " + qFontToCss(font));
	}

	QStringList t;
	t.reserve(tags.count());
	for (const Tag &tag : tags)
		t.append(stylished(tag, styles, count, noUnderscores));

	return t;
}

QString TagStylist::stylished(const Tag &tag, const QMap<QString, QString> &styles, bool count, bool noUnderscores) const
{
	// Guess the correct tag family
	const QString plural = tag.type().name() + "s";
	QString key = styles.contains(plural) ? plural : "generals";
	if (m_profile->getBlacklist().contains(QStringList() << tag.text()))
		key = "blacklisteds";
	if (m_profile->getIgnored().contains(tag.text(), Qt::CaseInsensitive))
		key = "ignoreds";
	for (const QString &t : qAsConst(m_profile->getKeptForLater()))
		if (t == tag.text())
			key = "keptForLater";
	for (const Favorite &fav : qAsConst(m_profile->getFavorites()))
		if (fav.getName() == tag.text())
			key = "favorites";

	QString txt = tag.text();
	QString ret = QString("<a href=\"%1\" style=\"%2\">%3</a>").arg(tag.text(), styles.value(key), noUnderscores ? txt.replace('_', ' ') : tag.text());
	if (count && tag.count() > 0)
		ret += QString(" <span style=\"color:#aaa\">(%L1)</span>").arg(tag.count());

	return ret;
}
