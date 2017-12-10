#ifndef TAG_DATABASE_H
#define TAG_DATABASE_H

#include <QMap>
#include <QString>
#include "tags/tag.h"
#include "tags/tag-type.h"


class TagDatabase
{
	public:
		void loadTypes();
		virtual bool load();
		virtual bool save() = 0;
		virtual void setTags(const QList<Tag> &tags) = 0;
		virtual QMap<QString, TagType> getTagTypes(const QStringList &tags) const = 0;
		virtual int count() const = 0;
		QMap<int, TagType> tagTypes() const;

	protected:
		explicit TagDatabase(const QString &typeFile);

	protected:
		QMap<int, TagType> m_tagTypes;

	private:
		QString m_typeFile;
};

#endif // TAG_DATABASE_H
