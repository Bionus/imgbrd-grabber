#ifndef TAG_DATABASE_IN_MEMORY_H
#define TAG_DATABASE_IN_MEMORY_H

#include "tag-database.h"
#include <QHash>


class TagDatabaseInMemory : public TagDatabase
{
	public:
		TagDatabaseInMemory(QString typeFile, QString tagFile);
		bool load() override;
		bool save();
		void setTags(const QList<Tag> &tags);
		QMap<QString, TagType> getTagTypes(QStringList tags) const;
		int count() const override;

	private:
		QString m_tagFile;
		QHash<QString, TagType> m_database;
};

#endif // TAG_DATABASE_IN_MEMORY_H
