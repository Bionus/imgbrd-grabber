#include "downloader/download-query-loader.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "logger.h"
#include "models/site.h"


bool DownloadQueryLoader::load(const QString &path, QList<DownloadQueryImage> &uniques, QList<DownloadQueryGroup> &groups, const QMap<QString, Site*> &sites)
{
	QFile f(path);
	if (!f.open(QFile::ReadOnly))
	{
		return false;
	}

	// Get the file's header to get the version
	QString header = f.readLine().trimmed();

	// Version 1 and 2 are plain text
	if (header.startsWith("[IGL "))
	{
		QString fieldSeparator(QChar(29));
		QString lineSeparator(QChar(28));

		// Read the remaining file
		QString links = f.readAll();
		f.close();
		QStringList det = links.split(lineSeparator, QString::SkipEmptyParts);
		if (det.empty())
			return false;

		for (const QString &link : det)
		{
			QStringList infos = link.split(fieldSeparator);
			if (infos.size() == 9)
			{
				QString source = infos[6];
				if (!sites.contains(source))
					continue;

				uniques.append(DownloadQueryImage(infos[0].toULongLong(), infos[1], infos[2], infos[3], infos[4], infos[5], sites[source], infos[7], infos[8]));
			}
			else
			{
				QString source = infos[5];
				if (!sites.contains(source) || infos.at(1).toInt() < 0 || infos.at(2).toInt() < 1 || infos.at(3).toInt() < 1)
					continue;

				groups.append(DownloadQueryGroup(
					infos[0],
					infos[1].toInt(),
					infos[2].toInt(),
					infos[3].toInt(),
					QStringList(),
					infos[4] != QLatin1String("false"),
					sites[source],
					infos[6],
					infos[7]
				));
			}
		}

		return true;
	}

	// Other versions are JSON-based
	f.reset();

	QByteArray data = f.readAll();
	QJsonDocument loadDoc = QJsonDocument::fromJson(data);
	QJsonObject object = loadDoc.object();

	int version = object["version"].toInt();
	switch (version)
	{
		case 3:
		{
			QJsonArray groupsJson = object["batchs"].toArray();
			for (auto groupJson : groupsJson)
			{
				DownloadQueryGroup batch;
				if (batch.read(groupJson.toObject(), sites))
					groups.append(batch);
			}

			QJsonArray uniquesJson = object["uniques"].toArray();
			for (auto uniqueJson : uniquesJson)
			{
				DownloadQueryImage unique;
				if (unique.read(uniqueJson.toObject(), sites))
					uniques.append(unique);
			}
			return true;
		}

		default:
			log(QStringLiteral("Unknown IGL file version: %1").arg(version), Logger::Warning);
			return false;
	}
}

bool DownloadQueryLoader::save(const QString &path, const QList<DownloadQueryImage> &uniques, const QList<DownloadQueryGroup> &groups)
{
	QFile saveFile(path);
	if (!saveFile.open(QFile::WriteOnly))
	{
		return false;
	}

	// Batch downloads
	QJsonArray groupsJson;
	for (const auto &b : groups)
	{
		QJsonObject batch;
		b.write(batch);
		groupsJson.append(batch);
	}

	// Unique images
	QJsonArray uniquesJson;
	for (const auto &u : uniques)
	{
		QJsonObject unique;
		u.write(unique);
		uniquesJson.append(unique);
	}

	// Generate result
	QJsonObject full;
	full["version"] = 3;
	full["batchs"] = groupsJson;
	full["uniques"] = uniquesJson;

	// Write result
	QJsonDocument saveDoc(full);
	saveFile.write(saveDoc.toJson());
	saveFile.close();

	return true;
}
