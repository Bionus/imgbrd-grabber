#ifndef TAG_H
#define TAG_H

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include "tags/tag-type.h"


class Tag
{
	public:
		Tag();
		explicit Tag(const QString &text, const QString &type = "unknown", int count = 0, const QStringList &related = QStringList());
		explicit Tag(const QString &text, const TagType &type, int count = 0, const QStringList &related = QStringList());
		explicit Tag(int id, const QString &text, TagType type, int count = 0, QStringList related = QStringList());
		static QString GetType(QString type, QMap<int, QString> ids = QMap<int, QString>());

		// Serialization
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json);

		// Setters
		void setId(int id);
		void setText(const QString &text);
		void setType(const TagType &type);
		void setCount(int count);
		void setRelated(const QStringList &related);

		// Getters
		int id() const;
		const QString &text() const;
		const TagType &type() const;
		int count() const;
		const QStringList &related() const;

	private:
		int m_id;
		QString m_text;
		TagType m_type;
		int m_count;
		QStringList m_related;
};

bool sortTagsByType(const Tag &, const Tag &);
bool sortTagsByName(const Tag &, const Tag &);
bool sortTagsByCount(const Tag &, const Tag &);

bool operator==(const Tag &t1, const Tag &t2);
bool operator!=(const Tag &t1, const Tag &t2);

Q_DECLARE_METATYPE(Tag)
Q_DECLARE_METATYPE(QList<Tag>)

#endif // TAG_H
