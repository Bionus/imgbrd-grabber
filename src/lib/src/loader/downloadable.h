#ifndef DOWNLOADABLE_H
#define DOWNLOADABLE_H

#include <QList>
#include <QMap>
#include <QPair>
#include <QString>


typedef QPair<QString, QString> QStrP;

class Filename;
class Profile;
class QColor;
class QStringList;
class QUrl;
class Token;

class Downloadable
{
	public:
		enum SaveResult
		{
			AlreadyExistsDisk,
			AlreadyExistsMd5,
			AlreadyExistsDeletedMd5,
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
			Unknown,
			Thumbnail,
			Sample,
			Full
		};

		virtual ~Downloadable() = default;
		virtual void preload(const Filename &filename) = 0;
		virtual QUrl url(Size size) const = 0;
		virtual QStringList paths(const Filename &filename, const QString &folder, int count) const = 0;
		const QMap<QString, Token> &tokens(Profile *profile) const;
		virtual SaveResult preSave(const QString &path, Size size) = 0;
		virtual void postSave(const QString &path, Size size, SaveResult result, bool addMd5, bool startCommands, int count) = 0;

		virtual QColor color() const = 0;
		virtual QString tooltip() const = 0;
		virtual QString counter() const = 0;
		virtual QList<QStrP> detailsData() const = 0;

		void refreshTokens();

	protected:
		virtual QMap<QString, Token> generateTokens(Profile *profile) const = 0;

	private:
		mutable QMap<QString, Token> m_tokens;
};

#endif // DOWNLOADABLE_H
