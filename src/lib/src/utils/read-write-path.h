#ifndef READ_WRITE_PATH_H
#define READ_WRITE_PATH_H

#include <QString>


class ReadWritePath
{
	public:
		ReadWritePath(QString readPath, QString writePath);

		QString readPath() const { return m_readPath; }
		QString readPath(const QString &path) const;

		QString writePath() const { return m_writePath; }
		QString writePath(const QString &path, bool createParent = false) const;

	private:
		QString m_readPath;
		QString m_writePath;
};

#endif // READ_WRITE_PATH_H
