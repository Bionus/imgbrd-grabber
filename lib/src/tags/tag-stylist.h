#ifndef TAG_STYLIST_H
#define TAG_STYLIST_H

#include <QFont>
#include <QList>
#include <QString>
#include "tags/tag.h"


class Profile;

class TagStylist
{
	public:
		explicit TagStylist(Profile *profile);
		QStringList stylished(QList<Tag> tags, bool count = false, bool noUnderscores = false, const QString &sort = "") const;
		QString stylished(const Tag &tag, bool count = false, bool noUnderscores = false) const;

	private:
		Profile *m_profile;
};

#endif // TAG_STYLIST_H
