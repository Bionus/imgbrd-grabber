#ifndef FILENAME_H
#define FILENAME_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>


class Site;
class Image;
class QJSEngine;
class QSettings;
class Profile;
class Token;

class Filename
{
	public:
		Filename() = default;
		explicit Filename(QString format);
		QString format() const;
		void setFormat(const QString &format);
		void setEscapeMethod(QString (*)(const QVariant &));

		QStringList path(const Image &img, Profile *profile, const QString &pth = "", int counter = 0, bool complex = true, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;
		QStringList path(QMap<QString, Token> tokens, Profile *profile, QString folder = "", int counter = 0, bool complex = true, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;

		bool isValid(Profile *profile = nullptr, QString *error = nullptr) const;
		bool needTemporaryFile(const QMap<QString, Token> &tokens) const;

		int needExactTags(Site *site, const QString &api = "") const;
		int needExactTags(const QStringList &forcedTokens = QStringList()) const;

		QList<QMap<QString, Token>> expandTokens(const QString &filename, QMap<QString, Token> tokens, QSettings *settings) const;
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
