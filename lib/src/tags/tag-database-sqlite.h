#ifndef TAG_DATABASE_SQLITE_H
#define TAG_DATABASE_SQLITE_H

#include "tag-database.h"
#include <QSqlDatabase>


class TagDatabaseSqlite : public TagDatabase
{
	public:
		TagDatabaseSqlite(QString typeFile, QString tagFile);
		bool load() override;
		bool save();
		void setTags(const QList<Tag> &tags);
		TagType getTagType(QString tag) const;

	private:
		QString m_tagFile;
		QSqlDatabase m_database;
};

#endif // TAG_DATABASE_SQLITE_H
