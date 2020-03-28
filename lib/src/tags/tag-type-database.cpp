#include "tag-type-database.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QtMath>
#include <utility>
#include "tag-type.h"


TagTypeDatabase::TagTypeDatabase(QString file)
	: m_file(std::move(file))
{}

bool TagTypeDatabase::load()
{
	if (!m_tagTypes.isEmpty()) {
		return true;
	}

	QFile f(m_file);
	if (!f.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}

	QTextStream in(&f);
	while (!in.atEnd()) {
		QString line = in.readLine();

		QStringList data = line.split(',');
		if (data.count() != 2) {
			continue;
		}

		const int id = data[0].toInt();
		m_tagTypes.insert(id, TagType(data[1]));
		m_invertedTagTypes.insert(data[1], id);
		m_maxTagTypeId = qMax(m_maxTagTypeId, id);
	}
	f.close();

	return true;
}

bool TagTypeDatabase::save()
{
	if (!m_pendingFlush) {
		return true;
	}
	return flush();
}

bool TagTypeDatabase::flush()
{
	QFile f(m_file);
	if (f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		return false;
	}

	for (auto it = m_tagTypes.constBegin(); it != m_tagTypes.constEnd(); ++it) {
		f.write(QString("%1,%2\n").arg(QString::number(it.key()), it.value().name()).toUtf8());
	}
	f.close();

	return true;
}

const QMap<int, TagType> &TagTypeDatabase::getAll() const
{
	return m_tagTypes;
}

void TagTypeDatabase::setAll(const QList<TagTypeWithId> &tagTypes)
{
	m_tagTypes.clear();
	for (const auto &tagType : tagTypes) {
		m_tagTypes.insert(tagType.id, TagType(tagType.name));
		m_invertedTagTypes.insert(tagType.name, tagType.id);
	}
	m_pendingFlush = true;
}


bool TagTypeDatabase::contains(int id) const
{
	return m_tagTypes.contains(id);
}

TagType TagTypeDatabase::get(int id) const
{
	return m_tagTypes.value(id);
}

int TagTypeDatabase::get(const TagType &tagType, bool create)
{
	const int ret = m_invertedTagTypes.value(tagType.name(), -1);
	if (ret == -1 && create) {
		return addTagType(tagType);
	}
	return ret;
}

int TagTypeDatabase::addTagType(const TagType &tagType)
{
	m_maxTagTypeId++;
	m_tagTypes.insert(m_maxTagTypeId, tagType);
	m_invertedTagTypes.insert(tagType.name(), m_maxTagTypeId);
	m_pendingFlush = true;
	return m_maxTagTypeId;
}
