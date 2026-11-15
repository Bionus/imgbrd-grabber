#include "image-magick.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include "functions.h"
#include "logger.h"


QString ImageMagick::version(int msecs)
{
	QProcess process;
	process.start("magick", { "-version" });

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

	if (line.startsWith("Version: ImageMagick ")) {
		line = line.mid(21);
	}

	const qsizetype index = line.indexOf("https://");
	if (index != -1) {
		return line.left(index).trimmed();
	}

	return line.trimmed();
}


QString ImageMagick::convert(const QString &file, const QString &extension, bool overwrite, bool deleteOriginal, int msecs)
{
	// Since the method takes an extension, build an absolute path to the input file with that extension
	const QFileInfo info(file);
	QString destination = info.path() + QDir::separator() + info.completeBaseName() + "." + extension;

	// Ensure the operation is safe to do
	if (!QFile::exists(file)) {
		log(QStringLiteral("Cannot convert file that does not exist: `%1`").arg(file), Logger::Error);
		return file;
	}
	if (QFile::exists(destination) && !overwrite) {
		log(QStringLiteral("Converting the file `%1` would overwrite another file: `%2`").arg(file, destination), Logger::Error);
		return file;
	}

	// Execute the conversion command
	const QStringList params = { file, destination };
	if (!execute(params, msecs)) {
		// Cleanup failed conversions
		if (QFile::exists(destination)) {
			log(QStringLiteral("Cleaning up failed conversion target file: `%1`").arg(destination), Logger::Warning);
			QFile::remove(destination);
		}

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


bool ImageMagick::execute(const QStringList &params, int msecs)
{
	QProcess process;
	process.start("magick", params);

	// Ensure the process started successfully
	if (!process.waitForStarted(msecs)) {
		log(QStringLiteral("Could not start ImageMagick"));
		return false;
	}

	// Wait for FFmpeg to finish
	bool finishedOk = process.waitForFinished(msecs);
	bool didntCrash = process.exitStatus() == QProcess::NormalExit;
	bool exitCodeOk = process.exitCode() == 0;
	bool ok = finishedOk && didntCrash && exitCodeOk;

	// Print stdout and stderr to the log
	const QString standardOutput = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
	if (!standardOutput.isEmpty()) {
		log(QString("[ImageMagick] %1").arg(standardOutput), Logger::Debug);
	}
	const QString standardError = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
	if (!standardError.isEmpty()) {
		log(QString("[ImageMagick] %1").arg(standardError), Logger::Error);
	}

	return ok;
}
