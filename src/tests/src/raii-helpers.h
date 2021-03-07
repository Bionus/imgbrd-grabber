#ifndef RAII_HELPERS_H
#define RAII_HELPERS_H

#include <QDir>
#include <QFile>
#include <QString>


class FileDeleter
{
	public:
		explicit FileDeleter(QString path, bool del = false)
			: m_path(std::move(path))
		{
			if (del && QFile::exists(m_path)) {
				QFile::remove(m_path);
			}
		}

		~FileDeleter()
		{
			QFile::remove(m_path);
		}

	private:
		QString m_path;
};

class DirectoryDeleter
{
	public:
		explicit DirectoryDeleter(QString path, bool create = true)
			: m_path(std::move(path))
		{
			if (create) {
				QDir().mkpath(m_path);
			}
		}

		~DirectoryDeleter()
		{
			QDir(m_path).removeRecursively();
		}

	private:
		QString m_path;
};

#endif // RAII_HELPERS_H
