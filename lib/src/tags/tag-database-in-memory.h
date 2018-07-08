#ifndef TAG_DATABASE_IN_MEMORY_H
#define TAG_DATABASE_IN_MEMORY_H

#include "tags/tag-database.h"
#include <QHash>


class TagDatabaseInMemory : public TagDatabase
{
	public:
		TagDatabaseInMemory(const QString &typeFile, QString tagFile);
		~TagDatabaseInMemory() override = default;
		bool load() override;
		bool save() override;
		void setTags(const QList<Tag> &tags) override;
		QMap<QString, TagType> getTagTypes(const QStringList &tags) const override;
		int count() const override;

	private:
		QString m_tagFile;
		QHash<QString, TagType> m_database;
};

#endif // TAG_DATABASE_IN_MEMORY_H
