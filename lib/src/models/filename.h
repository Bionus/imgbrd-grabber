#ifndef FILENAME_H
#define FILENAME_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QVariant>


class Site;
class Image;
class Profile;

class Filename
{
	public:
		Filename() = default;
		explicit Filename(QString format);
		QString getFormat() const;
		void setFormat(QString format);
		void setEscapeMethod(QString (*)(QVariant));

		/**
		 * Return the filename of the image according to the user's settings.
		 * @param fn The user's filename.
		 * @param pth The user's root save path.
		 * @param counter Current image count (used for batch downloads).
		 * @param complex Whether the filename is complex or not (contains conditionals).
		 * @return The filename of the image, with any token replaced.
		 */
		QStringList path(const Image& img, Profile *settings, QString pth = "", int counter = 0, bool complex = true, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;
		QStringList	path(QMap<QString, QVariant> tokens, Profile *settings, QString folder = "", int counter = 0, bool complex = true, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;

		/**
		 * Check filename format's validity.
		 * @param	error	The format to be validated.
		 * @return			Whether the filename is valid or not.
		 * @todo			Return a constant instead of a QString.
		 */
		bool isValid(Profile *profile = nullptr, QString *error = nullptr) const;

		bool needExactTags(Site *site, QString api = "") const;
		bool needExactTags(bool forceImageUrl = false, bool needDate = false) const;

		QList<QMap<QString, QVariant>> expandTokens(const QString &filename, QMap<QString, QVariant> tokens, QSettings *settings) const;
		QString expandConditionals(QString text, QStringList tags, const QMap<QString, QVariant> &tokens, QSettings *settings, int depth = 0) const;

	protected:
		QString cleanUpValue(QString res, QMap<QString, QString> options, QSettings *settings) const;
		QString optionedValue(const QVariant &val, QString key, QString ops, QSettings *settings, QStringList namespaces) const;
		QList<QVariant> getReplace(const QString &key, const QStringList &value, QSettings *settings) const;
		bool returnError(QString msg, QString *error) const;
		QString fixSeparator(QString separator) const;
		QString generateJavaScriptVariables(QSettings *settings, const QMap<QString, QVariant> &tokens) const;
		bool matchConditionalFilename(QString cond, QSettings *settings, const QMap<QString, QVariant> &tokens) const;

	private:
		QString m_format;
		QString (*m_escapeMethod)(QVariant) = nullptr;
};

#endif // FILENAME_H
