#ifndef DOWNLOADABLE_H
#define DOWNLOADABLE_H

#include <QColor>
#include <QList>
#include <QMap>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QUrl>


typedef QPair<QString, QString> QStrP;

class Filename;
class Profile;
class Token;

class Downloadable
{
	public:
		enum SaveResult
		{
			AlreadyExistsDisk,
			AlreadyExistsMd5,
			Blacklisted,
			Moved,
			Copied,
			Linked,
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

		virtual ~Downloadable() = default;
		virtual void preload(const Filename &filename) = 0;
		virtual QUrl url(Size size) const = 0;
		virtual QStringList paths(const Filename &filename, const QString &folder, int count) const = 0;
		const QMap<QString, Token> &tokens(Profile *profile) const;
		virtual SaveResult preSave(const QString &path) = 0;
		virtual void postSave(const QString &path, SaveResult result, bool addMd5, bool startCommands, int count) = 0;

		virtual QColor color() const = 0;
		virtual QString tooltip() const = 0;
		virtual QList<QStrP> detailsData() const = 0;

		void refreshTokens();

	protected:
		virtual QMap<QString, Token> generateTokens(Profile *profile) const = 0;

	private:
		mutable QMap<QString, Token> m_tokens;
};

#endif // DOWNLOADABLE_H
