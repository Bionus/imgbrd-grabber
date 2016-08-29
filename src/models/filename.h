#ifndef FILENAME_H
#define FILENAME_H

#include "image.h"


class Filename
{
	public:
		Filename(QString format);
		QString getFormat() const;

		/**
		 * Return the filename of the image according to the user's settings.
		 * @param fn The user's filename.
		 * @param pth The user's root save path.
		 * @param counter Current image count (used for batch downloads).
		 * @param complex Whether the filename is complex or not (contains conditionals).
		 * @return The filename of the image, with any token replaced.
		 */
		QStringList	path(const Image& img, QSettings *settings, QString pth = "", int counter = 0, bool complex = true, bool maxlength = true, bool shouldFixFilename = true, bool getFull = false) const;

		/**
		 * Check filename format's validity.
		 * @param	error	The format to be validated.
		 * @return			Whether the filename is valid or not.
		 * @todo			Return a constant instead of a QString.
		 */
		bool isValid(QString *error = nullptr) const;

		QList<QMap<QString, QPair<QString, QString>>> getReplaces(QString filename, const Image& img, QSettings *settings, QMap<QString,QStringList> custom) const;
		QString expandConditionals(QString text, QStringList tokens, QStringList tags, QMap<QString, QPair<QString, QString>> replaces, int depth = 0) const;
		QMap<QString, QStringList> makeDetails(const Image& img, QSettings *settings) const;

	protected:
		QList<QPair<QString,QString>> getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings) const;
		bool returnError(QString msg, QString *error) const;

	private:
		QString m_format;
};

#endif // FILENAME_H
