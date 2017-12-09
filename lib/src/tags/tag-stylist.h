#ifndef TAG_STYLIST_H
#define TAG_STYLIST_H

#include <QString>
#include <QFont>
#include <QList>
#include "tags/tag.h"


class Profile;

class TagStylist
{
	public:
		TagStylist(Profile *profile);
		QStringList stylished(QList<Tag> tags, bool count = false, bool noUnderscores = false, QString sort = "") const;
		QString stylished(const Tag &tag, bool count = false, bool noUnderscores = false) const;

	private:
		Profile *m_profile;
};

#endif // TAG_STYLIST_H
