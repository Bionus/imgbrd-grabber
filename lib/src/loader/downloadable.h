#ifndef DOWNLOADABLE_H
#define DOWNLOADABLE_H

#include <QString>
#include <QUrl>
#include "loader/token.h"
#include "models/filename.h"


typedef QPair<QString, QString> QStrP;

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

		enum Size
		{
			Thumbnail,
			Sample,
			Full
		};

		virtual void preload(const Filename &filename) = 0;
		virtual QString url(Size size) const = 0;
		virtual QStringList paths(const Filename &filename, const QString &folder, int count) const = 0;
		const QMap<QString, Token> &tokens(Profile *profile) const;
		virtual SaveResult preSave(const QString &path) = 0;
		virtual void postSave(QMap<QString, SaveResult> result, bool addMd5, bool startCommands, int count) = 0;

		virtual QColor color() const = 0;
		virtual QString tooltip() const = 0;
		virtual QList<QStrP> detailsData() const = 0;

	protected:
		virtual QMap<QString, Token> generateTokens(Profile *profile) const = 0;
		void refreshTokens();

	private:
		mutable QMap<QString, Token> m_tokens;
};

#endif // DOWNLOADABLE_H
