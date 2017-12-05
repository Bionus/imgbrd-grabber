#ifndef DOWNLOADABLE_H
#define DOWNLOADABLE_H

#include <QString>
#include <QUrl>
#include "models/filename.h"
#include "token.h"


class Downloadable
{
	public:
		enum SaveResult
		{
			AlreadyExists,
			Ignored,
			Moved,
			Copied,
			Saved,
			Error,
			NotLoaded,
			NotFound,
			NetworkError
		};

		virtual void preload(const Filename &filename) = 0;
		virtual QColor color() const = 0;
		virtual QString url() const = 0;
		virtual QStringList paths(const Filename &filename, const QString &folder, int count) const = 0;
		virtual QMap<QString, Token> tokens(Profile *profile) const = 0;
		virtual SaveResult preSave(const QString &path) = 0;
		virtual void postSave(QMap<QString, SaveResult> result, bool addMd5, bool startCommands, int count) = 0;
};

#endif // DOWNLOADABLE_H
