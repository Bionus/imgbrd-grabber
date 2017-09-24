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
		Filename();
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
		QStringList	path(const Image& img, Profile *settings, QString pth = "", int counter = 0, bool complex = true, bool maxlength = true, bool shouldFixFilename = true, bool getFull = false, bool keepInvalidTokens = false) const;

		/**
		 * Check filename format's validity.
		 * @param	error	The format to be validated.
		 * @return			Whether the filename is valid or not.
		 * @todo			Return a constant instead of a QString.
		 */
		bool isValid(Profile *profile = nullptr, QString *error = nullptr) const;

		bool needExactTags(Site *site, QString api = "") const;
		bool needExactTags(bool forceImageUrl = false, bool needDate = false) const;

		QList<QMap<QString, QPair<QString, QString>>> getReplaces(QString filename, const Image &img, Profile *profile, QMap<QString,QStringList> custom) const;
		QString expandConditionals(QString text, QStringList tokens, QStringList tags, QMap<QString, QPair<QString, QString>> replaces, QSettings *settings, int depth = 0) const;
		QMap<QString, QStringList> makeDetails(const Image& img, Profile *profile, QSettings *settings) const;

	protected:
		QString cleanUpValue(QString res, QMap<QString, QString> options, QSettings *settings) const;
		QString optionedValue(QString res, QString key, QString ops, const Image &img, QSettings *settings, QStringList namespaces) const;
		QList<QPair<QString,QString>> getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings) const;
		bool returnError(QString msg, QString *error) const;
		QString fixSeparator(QString separator) const;
		QString generateJavaScriptVariables(QSettings *settings, QMap<QString, QPair<QString, QString>> replaces) const;
		bool matchConditionalFilename(QString cond, QSettings *settings, QMap<QString, QPair<QString, QString>> replaces, QMap<QString, QStringList> details) const;

	private:
		QString m_format;
		QString (*m_escapeMethod)(QVariant) = nullptr;
};

#endif // FILENAME_H
