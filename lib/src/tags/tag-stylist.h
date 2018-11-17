#ifndef TAG_STYLIST_H
#define TAG_STYLIST_H

#include <QList>
#include <QMap>
#include <QString>


class Profile;
class Tag;

class TagStylist
{
	public:
		explicit TagStylist(Profile *profile);
		QStringList stylished(QList<Tag> tags, bool count = false, bool noUnderscores = false, const QString &sort = "") const;
		QString stylished(const Tag &tag, const QMap<QString, QString> &styles, bool count = false, bool noUnderscores = false) const;

	private:
		Profile *m_profile;
};

#endif // TAG_STYLIST_H
