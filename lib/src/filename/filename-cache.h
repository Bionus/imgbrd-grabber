#ifndef FILENAME_CACHE_H
#define FILENAME_CACHE_H

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include "filename/ast/filename-node-root.h"
#include "filename/filename-parser.h"


class FilenameCache
{
	public:
		static QSharedPointer<FilenameNodeRoot> Get(const QString &key);

	private:
		static QMap<QString, QWeakPointer<FilenameNodeRoot>> Map;
};


QMap<QString, QWeakPointer<FilenameNodeRoot>> FilenameCache::Map;

QSharedPointer<FilenameNodeRoot> FilenameCache::Get(const QString &key)
{
	auto it = Map.find(key);
	if (it != Map.end()) {
		return QSharedPointer<FilenameNodeRoot>(it.value());
	}

	FilenameParser parser(key);
	const QSharedPointer<FilenameNodeRoot> shared(parser.parseRoot());

	const QWeakPointer<FilenameNodeRoot> weak(shared);
	Map.insert(key, weak);

	return shared;
}

#endif // FILENAME_CACHE_H
