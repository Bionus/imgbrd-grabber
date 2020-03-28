#ifndef TAG_TYPE_DATABASE_H
#define TAG_TYPE_DATABASE_H

#include <QMap>
#include <QString>
#include "models/api/api.h"


class TagType;

class TagTypeDatabase
{
	public:
		explicit TagTypeDatabase(QString file);
		bool load();
		bool save();
		const QMap<int, TagType> &getAll() const;
		void setAll(const QList<TagTypeWithId> &tagTypes);

		bool contains(int id) const;
		TagType get(int id) const;
		int get(const TagType &tagType, bool create = false);

	protected:
		bool flush();
		int addTagType(const TagType &tagType);

	private:
		QString m_file;
		QMap<int, TagType> m_tagTypes;
		QMap<QString, int> m_invertedTagTypes;
		int m_maxTagTypeId = -1;
		bool m_pendingFlush = false;
};

#endif // TAG_TYPE_DATABASE_H
