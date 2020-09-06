#include "models/md5-database/md5-database-text.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <utility>
#include "logger.h"


Md5DatabaseText::Md5DatabaseText(QString path, QSettings *settings)
	: Md5Database(settings), m_path(std::move(path)), m_flushTimer(this)
{
	// Read all MD5 from the database and load them in memory
	log("Start loading MD5 database");
	QFile fileMD5(m_path);
	if (fileMD5.open(QFile::ReadOnly | QFile::Text)) {
		QString line;
		while (!(line = fileMD5.readLine()).isEmpty()) {
			m_md5s.insert(line.left(32), line.mid(32).trimmed());
		}

		fileMD5.close();
	}
	log(QString("MD5 database loaded (%1 entries)").arg(m_md5s.count()));

	// Connect the timer to the flush slot
	m_flushTimer.setSingleShot(true);
	m_flushTimer.setInterval(m_settings->value("md5_flush_interval", 1000).toInt());
	connect(&m_flushTimer, &QTimer::timeout, this, &Md5DatabaseText::flush);
}

Md5DatabaseText::~Md5DatabaseText()
{
	sync();
}


/**
 * Appends the newly-added MD5s to the MD5 file.
 */
void Md5DatabaseText::flush()
{
	if (m_path.isEmpty()) {
		return;
	}

	QFile fileMD5(m_path);
	if (fileMD5.open(QFile::Text | QFile::WriteOnly | QFile::Append)) {
		for (auto it = m_pendingAdd.begin(); it != m_pendingAdd.end(); ++it) {
			fileMD5.write(QString(it.key() + it.value() + "\n").toUtf8());
		}

		fileMD5.close();
	}

	m_pendingAdd.clear();
	emit flushed();
}

/**
 * Rewrites the whole contents of the MD5 file with the current database.
 */
void Md5DatabaseText::sync()
{
	if (m_path.isEmpty()) {
		return;
	}

	QFile fileMD5(m_path);
	if (fileMD5.open(QFile::Text | QFile::WriteOnly | QFile::Truncate)) {
		for (auto it = m_md5s.begin(); it != m_md5s.end(); ++it) {
			fileMD5.write(QString(it.key() + it.value() + "\n").toUtf8());
		}

		fileMD5.close();
	}

	m_pendingAdd.clear();
	m_flushTimer.stop();
}

/**
 * Adds a md5 to the _md5 map and adds it to the md5 file.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Md5DatabaseText::add(const QString &md5, const QString &path)
{
	if (md5.isEmpty() || m_md5s.contains(md5, path)) {
		return;
	}

	m_md5s.insert(md5, path);
	log(QString("Added MD5: %1").arg(md5), Logger::Debug);

	// Add MD5 to the "waiting to be saved" list
	m_pendingAdd.insert(md5, path);
	if (m_pendingAdd.count() >= 100) {
		m_flushTimer.stop();
		flush();
	} else {
		m_flushTimer.start();
	}
}

/**
 * Removes a md5 from the _md5 map and removes it from the md5 file.
 * @param	md5		The md5 to remove.
 */
void Md5DatabaseText::remove(const QString &md5, const QString &path)
{
	if (path.isEmpty()) {
		m_md5s.remove(md5);
	} else {
		m_md5s.remove(md5, path);
	}
}

/**
 * Returns all file paths associated to a given md5.
 * @param	md5		The md5 to look for.
 */
QStringList Md5DatabaseText::paths(const QString &md5)
{
	return m_md5s.values(md5);
}
