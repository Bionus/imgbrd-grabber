#include "downloader/download-query-loader.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "models/site.h"


bool DownloadQueryLoader::load(QString path, QList<DownloadQueryImage> &uniques, QList<DownloadQueryGroup> &batchs, const QMap<QString, Site*> &sites)
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

				uniques.append(DownloadQueryImage(infos[0].toInt(), infos[1], infos[2], infos[3], infos[4], infos[5], sites[source], infos[7], infos[8]));
			}
			else
			{
				QString source = infos[5];
				if (!sites.contains(source) || infos.at(1).toInt() < 0 || infos.at(2).toInt() < 1 || infos.at(3).toInt() < 1)
					continue;

				batchs.append(DownloadQueryGroup(infos[0], infos[1].toInt(), infos[2].toInt(), infos[3].toInt(), QStringList(), infos[4] != "false", sites[source], infos[6], infos[7]));
			}
		}

		return true;
	}

	// Other versions are JSON-based
	else
	{
		f.reset();

		QByteArray data = f.readAll();
		QJsonDocument loadDoc = QJsonDocument::fromJson(data);
		QJsonObject object = loadDoc.object();

		int version = object["version"].toInt();
		switch (version)
		{
			case 3:
				QJsonArray batchsJson = object["batchs"].toArray();
				for (auto batchJson : batchsJson)
				{
					DownloadQueryGroup batch;
					if (batch.read(batchJson.toObject(), sites))
						batchs.append(batch);
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
	}

	return false;
}

bool DownloadQueryLoader::save(QString path, const QList<DownloadQueryImage> &uniques, const QList<DownloadQueryGroup> &batchs)
{
	QFile saveFile(path);
	if (!saveFile.open(QFile::WriteOnly))
	{
		return false;
	}

	// Batch downloads
	QJsonArray batchsJson;
	for (const auto &b : batchs)
	{
		QJsonObject batch;
		b.write(batch);
		batchsJson.append(batch);
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
	full["batchs"] = batchsJson;
	full["uniques"] = uniquesJson;

	// Write result
	QJsonDocument saveDoc(full);
	saveFile.write(saveDoc.toJson());
	saveFile.close();

	return true;
}
