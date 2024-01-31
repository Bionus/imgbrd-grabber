#include "ffmpeg.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryDir>
#include "functions.h"
#include "logger.h"
#include "utils/zip.h"


QString FFmpeg::version(int msecs)
{
	QProcess process;
	process.start("ffmpeg", { "-version" });

	if (!process.waitForStarted(msecs)) {
		return "";
	}
	if (!process.waitForFinished(msecs)) {
		process.kill();
		return "";
	}
	if (process.exitCode() != 0) {
		return "";
	}

	const QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
	QString line = output.split("\n").first().trimmed();

	if (line.startsWith("ffmpeg version ")) {
		line = line.mid(15);
	}

	const qsizetype index = line.indexOf("Copyright");
	if (index != -1) {
		return line.left(index).trimmed();
	}

	return line.trimmed();
}


QString FFmpeg::remux(const QString &file, const QString &extension, bool deleteOriginal, int msecs)
{
	// Since the method takes an extension, build an absolute path to the input file with that extension
	const QFileInfo info(file);
	QString destination = info.path() + QDir::separator() + info.completeBaseName() + "." + extension;

	// Ensure the operation is safe to do
	if (!QFile::exists(file)) {
		log(QStringLiteral("Cannot remux file that does not exist: `%1`").arg(file), Logger::Error);
		return file;
	}
	if (QFile::exists(destination)) {
		log(QStringLiteral("Remuxing the file `%1` would overwrite another file: `%2`").arg(file, destination), Logger::Error);
		return file;
	}

	// Execute the conversion command
	const QStringList params = { "-n", "-loglevel", "error", "-i", file, "-c", "copy", destination };
	if (!execute(params, msecs)) {
		return file;
	}

	// Copy file creation information
	setFileCreationDate(destination, info.lastModified());

	// On success, delete the original file if requested
	if (deleteOriginal) {
		QFile::remove(file);
	}

	return destination;
}

QString FFmpeg::convertUgoira(const QString &file, const QList<QPair<QString, int>> &frameInformation, const QString &extension, bool deleteOriginal, int msecs)
{
	// Since the method takes an extension, build an absolute path to the input file with that extension
	const QFileInfo info(file);
	QString destination = info.path() + QDir::separator() + info.completeBaseName() + "." + extension;

	// Ensure the operation is safe to do
	if (info.suffix() != QStringLiteral("zip")) {
		log(QStringLiteral("Cannot convert ugoira file that is not a ZIP: `%1`").arg(file), Logger::Error);
		return file;
	}
	if (!QFile::exists(file)) {
		log(QStringLiteral("Cannot convert ugoira file that does not exist: `%1`").arg(file), Logger::Error);
		return file;
	}
	if (QFile::exists(destination)) {
		log(QStringLiteral("Converting the ugoira file `%1` would overwrite another file: `%2`").arg(file, destination), Logger::Error);
		return file;
	}

	// Extract the ugoira ZIP file
	QTemporaryDir tmpDir;
	const QString tmpDirPath = tmpDir.path();
	if (!tmpDir.isValid() || !unzipFile(file, tmpDir.path())) {
		log(QStringLiteral("Could not extract ugoira ZIP file `%1` into directory: `%2`").arg(file, destination), Logger::Error);
		return file;
	}

	// List all frame files from the ZIP
	QStringList frameFiles = QDir(tmpDir.path()).entryList(QDir::Files | QDir::NoDotAndDotDot);
	if (frameInformation.count() != frameFiles.count()) {
		log(QStringLiteral("Could not extract ugoira ZIP file `%1` into directory: `%2`").arg(file, destination), Logger::Error);
		return file;
	}

	// Build the ffmpeg concatenation string
	QFile ffconcatFile(tmpDir.filePath("ffconcat.txt"));
	if (!ffconcatFile.open(QFile::WriteOnly)) {
		log(QStringLiteral("Could not create temporary ffconcat file: `%1`").arg(ffconcatFile.fileName()), Logger::Error);
		return file;
	}
	QString ffconcat = "ffconcat version 1.0\n";
	for (const auto &frame : frameInformation) {
		ffconcat += "file " + (frame.first.isEmpty() ? frameFiles.takeFirst() : frame.first) + '\n';
		ffconcat += "duration " + QString::number(float(frame.second) / 1000) + '\n';
	}
	ffconcatFile.write(ffconcat.toUtf8());
	ffconcatFile.close();

	// Build the params
	QStringList params = { "-n", "-loglevel", "error", "-i", ffconcatFile.fileName() };
	if (extension == QStringLiteral("gif")) {
		params.append({ "-filter_complex", "[0:v]split[a][b];[a]palettegen=stats_mode=diff[p];[b][p]paletteuse=dither=bayer:bayer_scale=5:diff_mode=rectangle", "-vsync", "0" });
	} else if (extension == QStringLiteral("apng")) {
		params.append({ "-c:v", "apng", "-plays", "0", "-vsync", "0" });
	} else if (extension == QStringLiteral("webp")) {
		params.append({ "-c:v", "libwebp", "-lossless", "0", "-compression_level", "5", "-quality", "100", "-loop", "0", "-vsync", "0" });
	} else if (extension == QStringLiteral("webm")) {
		params.append({ "-c:v", "libvpx-vp9", "-lossless", "0", "-crf", "15", "-b", "0", "-vsync", "0" });
	} else {
		params.append({ "-c:v", "copy" });
	}
	params.append(destination);

	// Execute the conversion command
	if (!execute(params, msecs)) {
		return file;
	}

	// Copy file creation information
	setFileCreationDate(destination, info.lastModified());

	// On success, delete the original file if requested
	if (deleteOriginal) {
		QFile::remove(file);
	}

	return destination;
}


bool FFmpeg::execute(const QStringList &params, int msecs)
{
	QProcess process;
	process.start("ffmpeg", params);

	// Ensure the process started successfully
	if (!process.waitForStarted(msecs)) {
		log(QStringLiteral("Could not start FFmpeg"));
		return false;
	}

	// Wait for FFmpeg to finish
	bool ok = process.waitForFinished(msecs);

	// Print stdout and stderr to the log
	const QString standardOutput = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
	if (!standardOutput.isEmpty()) {
		log(QString("[Exiftool] %1").arg(standardOutput), Logger::Debug);
	}
	const QString standardError = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
	if (!standardError.isEmpty()) {
		log(QString("[Exiftool] %1").arg(standardError), Logger::Error);
	}

	return ok;
}
