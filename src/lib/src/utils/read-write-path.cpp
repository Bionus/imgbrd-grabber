#include "read-write-path.h"
#include <utility>
#include <QDir>
#include <QFile>
#include <QString>
#include "utils/file-utils.h"


ReadWritePath::ReadWritePath(const char *path)
	: ReadWritePath(QString(path))
{}
ReadWritePath::ReadWritePath(const QString &path)
	: m_readPath(path), m_writePath(path)
{}

ReadWritePath::ReadWritePath(QString readPath, QString writePath)
	: m_readPath(std::move(readPath)), m_writePath(std::move(writePath))
{}


QString ReadWritePath::readPath() const
{
	return m_readPath;
}
QString ReadWritePath::readPath(const QString &path) const
{
	QString writePath = this->writePath(path);
	if (QFile::exists(writePath)) {
		return writePath;
	}
	return QDir::cleanPath(m_readPath + QDir::separator() + path);
}

QString ReadWritePath::writePath() const
{
	return m_writePath;
}
QString ReadWritePath::writePath(const QString &path, bool createParent) const
{
	QString ret = m_writePath;
	if (!path.isEmpty()) {
		ret += QDir::separator() + path;
	}
	if (createParent) {
		ensureFileParent(ret);
	}
	return QDir::cleanPath(ret);
}

ReadWritePath ReadWritePath::readWritePath(const QString &path) const
{
	return {
		readPath(path),
		writePath(path),
	};
}
