#ifndef TAG_DATABASE_IN_MEMORY_H
#define TAG_DATABASE_IN_MEMORY_H

#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>
#include "tags/tag-database.h"


class ReadWritePath;
class Tag;
class TagType;

class TagDatabaseInMemory : public TagDatabase
{
	public:
		TagDatabaseInMemory(const ReadWritePath &typeFile, QString tagFile);
		~TagDatabaseInMemory() override = default;
		bool load() override;
		bool save() override;
		void setTags(const QList<Tag> &tags, bool createTagTypes = false) override;
		QMap<QString, TagType> getTagTypes(const QStringList &tags) const override;
		QMap<QString, int> getTagIds(const QStringList &tags) const override;
		int count() const override;

	private:
		QString m_tagFile;
		QHash<QString, TagType> m_database;
		mutable int m_count;
};

#endif // TAG_DATABASE_IN_MEMORY_H
