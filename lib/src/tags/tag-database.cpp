#include "tag-database.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <utility>
#include "tag-type.h"


TagDatabase::TagDatabase(QString typeFile)
	: m_typeFile(std::move(typeFile))
{}

bool TagDatabase::open()
{
	m_isOpen = true;
	return true;
}

bool TagDatabase::isOpen() const
{
	return m_isOpen;
}

bool TagDatabase::close()
{
	m_isOpen = false;
	return true;
}

bool TagDatabase::load()
{
	return loadTypes();
}

bool TagDatabase::loadTypes()
{
	if (!m_tagTypes.isEmpty()) {
		return true;
	}

	QFile f(m_typeFile);
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

		m_tagTypes.insert(data[0].toInt(), TagType(data[1]));
	}
	f.close();

	return true;
}

void TagDatabase::setTagTypes(const QList<TagTypeWithId> &tagTypes)
{
	m_tagTypes.clear();
	for (const auto &tagType : tagTypes) {
		m_tagTypes.insert(tagType.id, TagType(tagType.name));
	}

	QFile f(m_typeFile);
	if (f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		for (const auto &tagType : tagTypes) {
			f.write(QString("%1,%2\n").arg(QString::number(tagType.id), tagType.name).toUtf8());
		}
		f.close();
	}
}

const QMap<int, TagType> &TagDatabase::tagTypes() const
{
	return m_tagTypes;
}
