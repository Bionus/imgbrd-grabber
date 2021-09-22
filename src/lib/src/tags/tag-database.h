#ifndef TAG_DATABASE_H
#define TAG_DATABASE_H

#include <QMap>
#include "tags/tag-type-database.h"


class QString;
class Tag;
class TagType;
struct TagTypeWithId;

class TagDatabase
{
	public:
		virtual ~TagDatabase() = default;
		bool loadTypes();
		virtual bool open();
		bool isOpen() const;
		virtual bool close();
		virtual bool load();
		virtual bool save();
		virtual void setTags(const QList<Tag> &tags, bool createTagTypes = false) = 0;
		virtual void setTagTypes(const QList<TagTypeWithId> &tagTypes);
		virtual QMap<QString, TagType> getTagTypes(const QStringList &tags) const = 0;
		virtual QMap<QString, int> getTagIds(const QStringList &tags) const = 0;
		virtual int count() const = 0;
		const QMap<int, TagType> &tagTypes() const;

	protected:
		explicit TagDatabase(QString typeFile);

	protected:
		TagTypeDatabase m_tagTypeDatabase;
		bool m_isOpen = false;
};

#endif // TAG_DATABASE_H
