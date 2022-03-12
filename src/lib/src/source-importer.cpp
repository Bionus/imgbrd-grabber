#include "source-importer.h"
#include <QDir>
#include <QNetworkReply>
#include <QTemporaryDir>
#include <QUrl>
#include "functions.h"
#include "logger.h"
#include "models/profile.h"
#include "models/source.h"
#include "utils/read-write-path.h"
#include "utils/zip.h"


SourceImporter::SourceImporter(Profile *profile, QObject *parent)
	: QObject(parent), m_profile(profile), m_manager(new QNetworkAccessManager(this))
{
	connect(m_manager, &QNetworkAccessManager::finished, this, &SourceImporter::finishedLoading);
}

void SourceImporter::load(const QUrl &url) const
{
	m_manager->get(QNetworkRequest(url));
}

void SourceImporter::finishedLoading(QNetworkReply *reply)
{
	// Detect network errors
	if (reply->error() != QNetworkReply::NoError) {
		log(QStringLiteral("Network error %1 importing source: %2").arg(reply->error()).arg(reply->errorString()), Logger::Error);
		emit finished(ImportResult::NetworkError);
		return;
	}

	// Read response into a temporary file
	QFile tmpZip(m_profile->tempPath() + "/source-importer.zip");
	if (!tmpZip.open(QFile::WriteOnly)) {
		log(QStringLiteral("Could not open a temporary file to store source import ZIP"), Logger::Error);
		emit finished(ImportResult::ZipError);
		return;
	}
	tmpZip.write(reply->readAll());
	tmpZip.close();
	reply->deleteLater();

	// Create temporary directory to store the output
	QTemporaryDir tmpDir;
	if (!tmpDir.isValid()) {
		emit finished(ImportResult::ZipError);
		return;
	}

	// Unzip file
	unzipFile(tmpZip.fileName(), tmpDir.path());

	// Import valid sources
	QDir dir(tmpDir.path());
	if (dir.exists("model.js")) {
		importSource(tmpDir.path());
	} else {
		const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const auto &d: dirs) {
			importSource(d.filePath());
		}
	}

	emit finished(ImportResult::Success);
}

void SourceImporter::importSource(const QString &path)
{
	const QFileInfo d(path);

	// Ensure a "model.js" file exists in this directory
	QDir subDir(d.filePath());
	if (!subDir.exists("model.js")) {
		log(QStringLiteral("No 'model.js' file found in '%1'").arg(d.fileName()), Logger::Error);
		return;
	}

	// Ensure the Source has at least one API
	{
		Source tmpSource(m_profile, ReadWritePath(d.filePath()));
		if (tmpSource.getApis().isEmpty()) {
			log(QStringLiteral("Invalid source file in '%1'").arg(d.fileName()), Logger::Error);
			return;
		}
	}

	// Add the source to the profile, overwriting existing ones
	const QString dest = m_profile->getPath() + "/sites/" + d.fileName();
	copyRecursively(d.filePath(), dest, true);
	m_profile->addSource(new Source(m_profile, ReadWritePath(dest)));
}
