#ifndef FILENAME_H
#define FILENAME_H

#include <QFlags>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>


class AstFilename;
class Image;
class Profile;
class QSettings;
class Site;
class Token;

class Filename
{
	public:
		enum PathFlag
		{
			None = 0,
			ConditionalFilenames = 1, // complex (true)
			ExpandConditionals = 2, // complex (true)
			CapLength = 4, // maxLength (true)
			Fix = 8, // shouldFixFilename (true)
			IncludeFolder = 16, // getFull (false)
			KeepInvalidTokens = 32, // keepInvalidTokens (false)

			Complex = ConditionalFilenames | ExpandConditionals,
			File = CapLength | Fix,
			Path = File | IncludeFolder,
			Default = Complex | File,
		};
		Q_DECLARE_FLAGS(PathFlags, PathFlag)

		Filename();
		explicit Filename(QString format);
		QString format() const;
		void setFormat(const QString &format);
		void setEscapeMethod(QString (*)(const QVariant &));

		QStringList path(const Image &img, Profile *profile, const QString &pth = "", int counter = 0, PathFlags flags = Default) const;
		QStringList path(QMap<QString, Token> tokens, Profile *profile, QString folder = "", int counter = 0, PathFlags flags = Default) const;

		bool isValid(Profile *profile = nullptr, QString *error = nullptr) const;
		bool needTemporaryFile(const QMap<QString, Token> &tokens) const;

		int needExactTags(Site *site, const QString &api = "") const;
		int needExactTags(const QStringList &forcedTokens = QStringList()) const;

		QList<QMap<QString, Token>> expandTokens(QMap<QString, Token> tokens, QSettings *settings) const;

	protected:
		QList<Token> getReplace(const QString &key, const Token &token, QSettings *settings) const;
		bool returnError(const QString &msg, QString *error) const;

	private:
		QString m_format;
		QSharedPointer<AstFilename> m_ast;
		QString (*m_escapeMethod)(const QVariant &) = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Filename::PathFlags)

#endif // FILENAME_H
