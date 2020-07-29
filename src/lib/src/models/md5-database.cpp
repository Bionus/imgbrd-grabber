#include "models/md5-database.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <utility>
#include "logger.h"


Md5Database::Md5Database(QString path, QSettings *settings)
	: m_path(std::move(path)), m_settings(settings), m_flushTimer(this)
{
	// Read all MD5 from the database and load them in memory
	QFile fileMD5(m_path);
	if (fileMD5.open(QFile::ReadOnly | QFile::Text)) {
		QString line;
		while (!(line = fileMD5.readLine()).isEmpty()) {
			m_md5s.insert(line.left(32), line.mid(32).trimmed());
		}

		fileMD5.close();
	}

	// Connect the timer to the flush slot
	m_flushTimer.setSingleShot(true);
	m_flushTimer.setInterval(m_settings->value("md5_flush_interval", 1000).toInt());
	connect(&m_flushTimer, &QTimer::timeout, this, &Md5Database::flush);
}

Md5Database::~Md5Database()
{
	sync();
}


/**
 * Appends the newly-added MD5s to the MD5 file.
 */
void Md5Database::flush()
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
void Md5Database::sync()
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

QPair<QString, QString> Md5Database::action(const QString &md5, const QString &target)
{
	// If the MD5 is not found, just save the image
	if (md5.isEmpty() || !m_md5s.contains(md5)) {
		return { "save", "" };
	}

	// Get all existing paths for this MD5
	const QStringList paths = m_md5s.values(md5);

	// Split paths into "same dir" and "not same dir"
	QStringList sameDirPaths, notSameDirPaths;
	const QDir targetDir = QFileInfo(target).dir();
	for (const QString &path : paths) {
		if (!target.isEmpty() && targetDir == QFileInfo(path).dir()) {
			sameDirPaths.append(path);
		} else {
			notSameDirPaths.append(path);
		}
	}

	// Prioritize paths in the same directory for the action
	if (!sameDirPaths.isEmpty()) {
		const QString setting = m_settings->value("Save/md5DuplicatesSameDir", "save").toString();
		const QPair<QString, QString> ret = action(md5, sameDirPaths, setting);
		if (!ret.first.isEmpty()) {
			return ret;
		}
	}

	// Fallback to duplicates found in other places
	if (!notSameDirPaths.isEmpty()) {
		const QString setting = m_settings->value("Save/md5Duplicates", "save").toString();
		const QPair<QString, QString> ret = action(md5, notSameDirPaths, setting);
		if (!ret.first.isEmpty()) {
			return ret;
		}
	}

	// Just save by default
	return { "save", "" };
}

QPair<QString, QString> Md5Database::action(const QString &md5, const QStringList &paths, QString action)
{
	// If one of the files already exists, return its path with the relevant action
	const bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();
	for (const QString &path : paths) {
		if (QFile::exists(path)) {
			return { action, path };
		} else if (!keepDeleted) {
			remove(md5, path);
		}
	}

	// If we want to keep deleted files, we can't return a "move" action or similar, as there are no files to move
	if (keepDeleted) {
		QString path = paths.first(); // The actual path is irrelevant since none exist
		if (action != "ignore") {
			action = "save";
		}
		return { action, path };
	}

	return { "", "" };
}

/**
 * Check if a file with this md5 already exists;
 * @param	md5		The md5 that needs to be checked.
 * @return			A QString containing the path to the already existing file, an empty QString if the md5 does not already exists.
 */
QStringList Md5Database::exists(const QString &md5)
{
	QStringList ret;

	if (md5.isEmpty() || !m_md5s.contains(md5)) {
		return ret;
	}

	const bool keepDeleted = m_settings->value("Save/keepDeletedMd5", false).toBool();

	const QStringList paths = m_md5s.values(md5);
	for (const QString &path : paths) {
		if (QFile::exists(path) || keepDeleted) {
			ret.append(path);
		} else {
			remove(md5, path);
		}
	}

	return ret;
}

/**
 * Adds a md5 to the _md5 map and adds it to the md5 file.
 * @param	md5		The md5 to add.
 * @param	path	The path to the image with this md5.
 */
void Md5Database::add(const QString &md5, const QString &path)
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
void Md5Database::remove(const QString &md5, const QString &path)
{
	if (path.isEmpty()) {
		m_md5s.remove(md5);
	} else {
		m_md5s.remove(md5, path);
	}
}
