#include "ffmpeg.h"
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include "logger.h"


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
	const QString destination = info.path() + QDir::separator() + info.completeBaseName() + "." + extension;

	// Ensure the operation is safe to do
	if (!QFile::exists(file)) {
		log(QStringLiteral("Cannot remux file that does not exist: `%1`").arg(file), Logger::Error);
		return file;
	}
	if (QFile::exists(destination)) {
		log(QStringLiteral("Remuxing the file `%1` would overwrite another file: `%2`").arg(file, destination), Logger::Error);
		return file;
	}

	QProcess process;
	process.start("ffmpeg", { "-n", "-loglevel", "error", "-i", file, "-c", "copy", destination });

	// Ensure the process started successfully
	if (!process.waitForStarted(msecs)) {
		log(QStringLiteral("Could not start FFmpeg"));
		return file;
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

	// On success, delete the original file if requested
	if (ok && deleteOriginal) {
		QFile::remove(file);
	}

	return destination;
}
