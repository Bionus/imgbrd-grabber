#include "tags/tag-stylist.h"
#include "functions.h"
#include "models/profile.h"


TagStylist::TagStylist(Profile *profile)
	: m_profile(profile)
{}

QStringList TagStylist::stylished(QList<Tag> tags, bool count, bool noUnderscores, const QString &sort) const
{
	// Sort tag list
	if (sort == "type")
		std::sort(tags.begin(), tags.end(), sortTagsByType);
	else if (sort == "name")
		std::sort(tags.begin(), tags.end(), sortTagsByName);
	else if (sort == "count")
		std::sort(tags.begin(), tags.end(), sortTagsByCount);

	QStringList t;
	t.reserve(tags.count());
	for (const Tag &tag : tags)
		t.append(stylished(tag, count, noUnderscores));

	return t;
}

/**
 * Return the colored tag.
 * @param favs The list of the user's favorite tags.
 * @return The HTML colored tag.
 */
QString TagStylist::stylished(const Tag &tag, bool count, bool noUnderscores) const
{
	static const QStringList tlist = QStringList() << "artists" << "circles" << "copyrights" << "characters" << "models" << "generals" << "favorites" << "keptForLater" << "blacklisteds" << "ignoreds" << "favorites";
	static const QStringList defaults = QStringList() << "#aa0000" << "#55bbff" << "#aa00aa" << "#00aa00" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000" << "#000000" << "#999999" << "#ffcccc";

	// Guess the correct tag family
	QString key = tlist.contains(tag.type().name()+"s") ? tag.type().name() + "s" : "generals";
	if (m_profile->getBlacklist().contains(tag.text(), Qt::CaseInsensitive))
		key = "blacklisteds";
	if (m_profile->getIgnored().contains(tag.text(), Qt::CaseInsensitive))
		key = "ignoreds";
	for (const QString &t : m_profile->getKeptForLater())
		if (t == tag.text())
			key = "keptForLater";
	for (const Favorite &fav : m_profile->getFavorites())
		if (fav.getName() == tag.text())
			key = "favorites";

	QFont font;
	font.fromString(m_profile->getSettings()->value("Coloring/Fonts/" + key).toString());
	QString color = m_profile->getSettings()->value("Coloring/Colors/" + key, defaults.at(tlist.indexOf(key))).toString();
	QString style = "color:" + color + "; " + qFontToCss(font);

	QString ret;
	ret = "<a href=\"" + tag.text() + "\" style=\"" + style + "\">" + (noUnderscores ? tag.text().replace('_', ' ') : tag.text()) + "</a>";
	if (count && tag.count() > 0)
		ret += " <span style=\"color:#aaa\">(" + QString("%L1").arg(tag.count()) + ")</span>";

	return ret;
}
