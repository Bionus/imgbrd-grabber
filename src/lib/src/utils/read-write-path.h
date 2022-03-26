#ifndef READ_WRITE_PATH_H
#define READ_WRITE_PATH_H

#include <QString>


class ReadWritePath
{
	public:
		ReadWritePath(const char *path);
		ReadWritePath(const QString &path);
		ReadWritePath(QString readPath, QString writePath);

		QString readPath() const;
		QString readPath(const QString &path) const;

		QString writePath() const;
		QString writePath(const QString &path, bool createParent = false) const;

		ReadWritePath readWritePath(const QString &path) const;

	private:
		QString m_readPath;
		QString m_writePath;
};

#endif // READ_WRITE_PATH_H
