#ifndef FFMPEG_H
#define FFMPEG_H

#include <QString>


class FFmpeg
{
	public:
		/**
		 * Get the version of FFmpeg.
		 *
		 * @param msecs The duration to wait in milliseconds for the version command to run.
		 * @return The version number found, with basic parsing done (ex: "4.4.3").
		 */
		static QString version(int msecs = 30000);

		/**
		 * Remux a file to a different format, copying the streams.
		 *
		 * @param file The file to remux.
		 * @param extension The target extension (ex: "mp4").
		 * @param deleteOriginal Whether to delete the original file on success.
		 * @param msecs The duration to wait in milliseconds for the command to run.
		 * @return The destination file path on success, the original file path on error.
		 */
		static QString remux(const QString &file, const QString &extension, bool deleteOriginal = true, int msecs = 30000);

		/**
		 * Convert a ugoira ZIP file to a different format.
		 *
		 * @param file The file to remux.
		 * @param frameInformation A list of (frameFile, delay) tuples representing each frame in this ugoira ZIP file.
		 * @param extension The target extension (ex: "gif").
		 * @param deleteOriginal Whether to delete the original file on success.
		 * @param msecs The duration to wait in milliseconds for the command to run.
		 * @return The destination file path on success if the original was deleted, the original file path otherwise.
		 */
		static QString convertUgoira(const QString &file, const QList<QPair<QString, int>> &frameInformation, const QString &extension, bool deleteOriginal = true, int msecs = 30000);

	protected:
		static bool execute(const QStringList &params, int msecs = 30000);
};

#endif // FFMPEG_H
