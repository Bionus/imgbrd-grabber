#include "tags/tag-database-in-memory.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <utility>
#include "tags/tag.h"


TagDatabaseInMemory::TagDatabaseInMemory(const QString &typeFile, QString tagFile)
	: TagDatabase(typeFile), m_tagFile(std::move(tagFile)), m_count(-1)
{}

bool TagDatabaseInMemory::load()
{
	// Don't reload databases
	if (!m_database.isEmpty()) {
		return true;
	}

	// Load tag types
	if (!TagDatabase::load()) {
		return false;
	}

	QFile file(m_tagFile);
	if (!file.exists()) {
		return true;
	}
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();

		QStringList data = line.split(',');
		if (data.count() != 2) {
			continue;
		}

		int tId = data[1].toInt();
		if (!m_tagTypeDatabase.contains(tId)) {
			continue;
		}

		QString tag = data[0];
		if (tag.isEmpty()) {
			continue;
		}
		tag.squeeze();

		m_database.insert(tag, m_tagTypeDatabase.get(tId));
	}
	file.close();
	m_database.squeeze();

	return true;
}

bool TagDatabaseInMemory::save()
{
	QFile file(m_tagFile);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
		return false;
	}

	QHashIterator<QString, TagType> i(m_database);
	while (i.hasNext()) {
		i.next();

		TagType tagType = i.value();
		const int tagTypeId = m_tagTypeDatabase.get(tagType);
		if (tagTypeId == -1)
			continue;

		file.write(QString(i.key() + "," + QString::number(tagTypeId) + "\n").toUtf8());
	}
	file.close();

	return TagDatabase::save();
}

void TagDatabaseInMemory::setTags(const QList<Tag> &tags, bool createTagTypes)
{
	m_database.clear();
	for (const Tag &tag : tags) {
		m_database.insert(tag.text(), tag.type());
		if (createTagTypes) {
			m_tagTypeDatabase.get(tag.type(), true);
		}
	}
}

QMap<QString, TagType> TagDatabaseInMemory::getTagTypes(const QStringList &tags) const
{
	QMap<QString, TagType> ret;
	for (const QString &tag : tags) {
		if (m_database.contains(tag)) {
			ret.insert(tag, m_database[tag]);
		}
	}

	return ret;
}

int TagDatabaseInMemory::count() const
{
	if (!m_database.isEmpty()) {
		return m_database.count();
	}

	if (m_count != -1) {
		return m_count;
	}

	m_count = 0;

	QFile file(m_tagFile);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return m_count;
	}

	QTextStream in(&file);
	while (!in.atEnd()) {
		m_count++;
		in.readLine();
	}
	file.close();

	return m_count;
}
