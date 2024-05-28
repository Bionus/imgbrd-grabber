#ifndef IMAGE_MAGICK_H
#define IMAGE_MAGICK_H

#include <QString>


class ImageMagick
{
	public:
		/**
		 * Get the version of ImageMagick.
		 *
		 * @param msecs The duration to wait in milliseconds for the version command to run.
		 * @return The version number found (ex: "7.0.10").
		 */
		static QString version(int msecs = 30000);

		/**
		 * Convert a file to a different format.
		 *
		 * @param file The file to convert.
		 * @param extension The target extension (ex: "jpg").
		 * @param deleteOriginal Whether to delete the original file on success.
		 * @param msecs The duration to wait in milliseconds for the command to run.
		 * @return The destination file path on success, the original file path on error.
		 */
		static QString convert(const QString &file, const QString &extension, bool deleteOriginal = true, int msecs = 30000);

	protected:
		static bool execute(const QStringList &params, int msecs = 30000);
};

#endif // IMAGE_MAGICK_H
