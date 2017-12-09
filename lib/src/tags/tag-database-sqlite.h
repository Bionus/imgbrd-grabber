#ifndef TAG_DATABASE_SQLITE_H
#define TAG_DATABASE_SQLITE_H

#include "tags/tag-database.h"
#include <QSqlDatabase>


class TagDatabaseSqlite : public TagDatabase
{
	public:
		TagDatabaseSqlite(QString typeFile, QString tagFile);
		bool load() override;
		bool save() override;
		void setTags(const QList<Tag> &tags) override;
		QMap<QString, TagType> getTagTypes(QStringList tags) const override;
		int count() const override;

	private:
		QString m_tagFile;
		QSqlDatabase m_database;
		mutable int m_count;
};

#endif // TAG_DATABASE_SQLITE_H
