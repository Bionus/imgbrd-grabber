#ifndef TAG_DATABASE_H
#define TAG_DATABASE_H

#include <QMap>
#include <QString>
#include "models/api/api.h"
#include "tags/tag-type.h"


class Tag;

class TagDatabase
{
	public:
		virtual ~TagDatabase() = default;
		bool loadTypes();
		virtual bool open();
		bool isOpen() const;
		virtual bool close();
		virtual bool load();
		virtual bool save() = 0;
		virtual void setTags(const QList<Tag> &tags) = 0;
		virtual void setTagTypes(const QList<TagTypeWithId> &tagTypes);
		virtual QMap<QString, TagType> getTagTypes(const QStringList &tags) const = 0;
		virtual int count() const = 0;
		const QMap<int, TagType> &tagTypes() const;

	protected:
		explicit TagDatabase(QString typeFile);

	protected:
		QMap<int, TagType> m_tagTypes;
		bool m_isOpen = false;

	private:
		QString m_typeFile;
};

#endif // TAG_DATABASE_H
