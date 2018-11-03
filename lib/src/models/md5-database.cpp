#include "models/md5-database.h"
#include <QFile>
#include <QSettings>


Md5Database::Md5Database(QString path, QSettings *settings)
	: m_path(std::move(path)), m_settings(settings)
{
	QFile fileMD5(m_path);
	if (fileMD5.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while (!(line = fileMD5.readLine()).isEmpty())
			m_md5s.insert(line.left(32), line.mid(32).trimmed());

		fileMD5.close();
	}
}

Md5Database::~Md5Database()
{
	sync();
}


void Md5Database::sync()
{
	if (m_path.isEmpty())
		return;

	// MD5s
	QFile fileMD5(m_path);
	if (fileMD5.open(QFile::WriteOnly | QFile::Truncate))
	{
		QStringList md5s = m_md5s.keys();
		QStringList paths = m_md5s.values();
		for (int i = 0; i < md5s.size(); i++)
			fileMD5.write(QString(md5s[i] + paths[i] + "\n").toUtf8());

		fileMD5.close();
	}
}

QPair<QString, QString> Md5Database::action(const QString &md5)
{
	QString action = m_settings->value("Save/md5Duplicates", "save").toString();
	const bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();

	const bool contains = !md5.isEmpty() && m_md5s.contains(md5);
	QString path = contains ? m_md5s[md5] : QString();
	const bool exists = contains && QFile::exists(path);

	if (contains && !exists)
	{
		if (!keepDeleted)
		{
			remove(md5);
			path = QString();
		}
		else
		{ action = "ignore"; }
	}

	return QPair<QString, QString>(action, path);
}

/**
 * Check if a file with this md5 already exists;
 * @param	md5		The md5 that needs to be checked.
 * @return			A QString containing the path to the already existing file, an empty QString if the md5 does not already exists.
 */
QString Md5Database::exists(const QString &md5)
{
	if (m_md5s.contains(md5))
	{
		if (QFile::exists(m_md5s[md5]))
			return m_md5s[md5];

		if (!m_settings->value("Save/keepDeletedMd5", false).toBool())
			remove(md5);
	}
	return QString();
}

/**
 * Adds a md5 to the _md5 map and adds it to the md5 file.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Md5Database::add(const QString &md5, const QString &path)
{
	if (!md5.isEmpty())
	{ m_md5s.insert(md5, path); }
}

/**
 * Set a md5 to the _md5 map changing the file it is pointing to.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Md5Database::set(const QString &md5, const QString &path)
{
	m_md5s[md5] = path;
}

/**
 * Removes a md5 from the _md5 map and removes it from the md5 file.
 * @param	md5		The md5 to remove.
 */
void Md5Database::remove(const QString &md5)
{
	m_md5s.remove(md5);
}
