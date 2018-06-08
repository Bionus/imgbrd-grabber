#ifndef FILENAME_H
#define FILENAME_H

#include <QJSEngine>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>


class Site;
class Image;
class Profile;
class Token;

class Filename
{
	public:
		Filename() = default;
		explicit Filename(const QString &format);
		QString getFormat() const;
		void setFormat(const QString &format);
		void setEscapeMethod(QString (*)(const QVariant &));

		/**
		 * Return the filename of the image according to the user's settings.
		 * @param fn The user's filename.
		 * @param pth The user's root save path.
		 * @param counter Current image count (used for batch downloads).
		 * @param complex Whether the filename is complex or not (contains conditionals).
		 * @return The filename of the image, with any token replaced.
		 */
		QStringList path(const Image& img, Profile *settings, const QString &pth = "", int counter = 0, bool complex = true, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;
		QStringList	path(QMap<QString, Token> tokens, Profile *settings, QString folder = "", int counter = 0, bool complex = true, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;

		/**
		 * Check filename format's validity.
		 * @param	error	The format to be validated.
		 * @return			Whether the filename is valid or not.
		 * @todo			Return a constant instead of a QString.
		 */
		bool isValid(Profile *profile = nullptr, QString *error = nullptr) const;

		int needExactTags(Site *site, const QString &api = "") const;
		int needExactTags(QStringList forcedTokens = QStringList()) const;

		QList<QMap<QString, Token> > expandTokens(const QString &filename, QMap<QString, Token> tokens, QSettings *settings) const;
		QString expandConditionals(const QString &text, const QStringList &tags, const QMap<QString, Token> &tokens, QSettings *settings, int depth = 0) const;

	protected:
		QString cleanUpValue(QString res, const QMap<QString, QString> &options, QSettings *settings) const;
		QString optionedValue(const QVariant &val, const QString &key, const QString &ops, QSettings *settings, QStringList namespaces) const;
		QList<Token> getReplace(const QString &key, const Token &token, QSettings *settings) const;
		bool returnError(const QString &msg, QString *error) const;
		QString fixSeparator(const QString &separator) const;
		void setJavaScriptVariables(QJSEngine &engine, QSettings *settings, const QMap<QString, Token> &tokens) const;
		bool matchConditionalFilename(QString cond, QSettings *settings, const QMap<QString, Token> &tokens) const;

	private:
		QString m_format;
		QString (*m_escapeMethod)(const QVariant &) = nullptr;
};

#endif // FILENAME_H
