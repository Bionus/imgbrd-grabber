#ifndef TAG_DATABASE_SQLITE_H
#define TAG_DATABASE_SQLITE_H

#include <QHash>
#include <QSqlDatabase>
#include "tags/tag-database.h"
#include "tags/tag-type.h"


class TagDatabaseSqlite : public TagDatabase
{
	public:
		TagDatabaseSqlite(const QString &typeFile, const QString &tagFile);
		~TagDatabaseSqlite() override;
		bool load() override;
		bool save() override;
		void setTags(const QList<Tag> &tags) override;
		QMap<QString, TagType> getTagTypes(const QStringList &tags) const override;
		int count() const override;

	private:
		QString m_tagFile;
		QSqlDatabase m_database;
		mutable QHash<QString, TagType> m_cache;
		mutable int m_count;
};

#endif // TAG_DATABASE_SQLITE_H
