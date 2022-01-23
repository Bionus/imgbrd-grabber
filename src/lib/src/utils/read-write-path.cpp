#include "read-write-path.h"
#include <utility>
#include <QDir>
#include <QFile>
#include <QString>
#include "utils/file-utils.h"


ReadWritePath::ReadWritePath(QString readPath, QString writePath)
	: m_readPath(std::move(readPath)), m_writePath(std::move(writePath))
{}


QString ReadWritePath::readPath(const QString &path) const
{
	QString writePath = m_writePath + QDir::separator() + path;
	if (QFile::exists(writePath)) {
		return writePath;
	}
	return m_readPath + QDir::separator() + path;
}

QString ReadWritePath::writePath(const QString &path, bool createParent) const
{
	QString ret = m_writePath + QDir::separator() + path;
	if (createParent) {
		ensureFileParent(ret);
	}
	return ret;
}
