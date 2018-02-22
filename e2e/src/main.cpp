#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QNetworkAccessManager>
#include "functions.h"
#include "models/api/api.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


bool opCompare(QString op, int left, int right)
{
	if (right == -1)
		return true;
	if (op == ">")
		return left > right;
	if (op == "<")
		return left < right;
	return left == right;
}

bool jsonCompare(QVariant value, QJsonValue opt)
{
	QString op = "=";
	if (opt.isArray())
	{
		QJsonArray arrOpt = opt.toArray();
		op = arrOpt[0].toString();
		opt = arrOpt[1];
	}

	if (value.type() == QVariant::String)
	{ return value.toString() == opt.toString(); }

	return opCompare(op, value.toInt(), opt.toDouble());
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QCommandLineParser parser;
	parser.addHelpOption();

	QCommandLineOption inputOption(QStringList() << "i" << "input", "Input JSON configuration file", "input");
	QCommandLineOption outputOption(QStringList() << "o" << "output", "Output JSON result file", "output");
	parser.addOption(inputOption);
	parser.addOption(outputOption);
	parser.process(app);

	parser.value(inputOption);
	parser.value(outputOption);

	QFile f(parser.value(inputOption));
	if (!f.open(QFile::ReadOnly | QFile::Text))
		return 1;

	QJsonObject allJson;
	QJsonDocument input = QJsonDocument::fromJson(f.readAll());
	f.close();

	auto manager = new QNetworkAccessManager();
	Profile *profile = new Profile(savePath());
	auto allSources = profile->getSources();
	auto allSites = profile->getSites();

	QJsonObject sources = input.object();
	for (const QString &sourceName : sources.keys())
	{
		qDebug() << "#" << "Source" << sourceName;
		QJsonObject sourceJson;

		Source *source = allSources.value(sourceName);
		QJsonObject sites = sources.value(sourceName).toObject();

		QJsonObject sourceApis = sites.value("apis").toObject();
		QJsonArray sourceSearch = sites.value("search").toArray();;

		for (Site *site : source->getSites())
		{
			qDebug() << "##" << "Site" << site->url();
			QJsonObject siteJson;

			QJsonObject siteApis = sourceApis;
			QJsonArray siteSearch = sourceSearch;
			if (sourceJson.contains(site->url()))
			{
				QJsonObject override = sourceJson.value(site->url()).toObject();
				if (override.contains("apis"))
				{ siteApis = override.value("apis").toObject(); }
				if (override.contains("search"))
				{ siteSearch = override.value("search").toArray(); }
			}

			QString search = siteSearch[0].toString();
			int pagei = siteSearch[1].toDouble();
			int limit = siteSearch[2].toDouble();

			Page *page = new Page(profile, site, allSites.values(), QStringList() << search, pagei, limit);

			for (const QString &apiName : siteApis.keys())
			{
				qDebug() << "###" << "API" << apiName;
				QJsonObject apiJson;

				Api *api = Q_NULLPTR;
				for (Api *a : site->getApis())
					if (a->getName().toLower() == apiName.toLower())
						api = a;

				QString url = api->pageUrl(search, pagei, limit, 0, 0, 0, site);
				QEventLoop loop;
				auto reply = manager->get(QNetworkRequest(QUrl(url)));
				QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
				loop.exec();

				QString src = reply->readAll();
				auto ret = api->parsePage(page, src, 0);

				apiJson["status"] = "ok";
				QStringList message;

				// Checks
				QJsonArray checks = siteApis.value(apiName).toArray();
				if (!jsonCompare(ret.error, checks[0]))
				{
					apiJson["status"] = "error";
					message.append(ret.error);
				}
				if (!jsonCompare(ret.imagesCount, checks[1]))
				{
					if (apiJson["status"] == "ok")
					{ apiJson["status"] = "warning"; }
					message.append("Image count error: " + QString::number(ret.imagesCount));
				}
				if (!jsonCompare(ret.images.count(), checks[2]))
				{
					apiJson["status"] = "error";
					message.append("Number of images error: " + QString::number(ret.images.count()));
				}
				if (!jsonCompare(ret.tags.count(), checks[3]))
				{
					if (apiJson["status"] == "ok")
					{ apiJson["status"] = "warning"; }
					message.append("Number of tags error: " + QString::number(ret.tags.count()));
				}

				if (!message.isEmpty())
				{
					apiJson["message"] = message.join(", ");
					siteJson[apiName] = apiJson;
				}
				else
				{ siteJson[apiName] = apiJson["status"]; }
			}
			sourceJson[site->url()] = siteJson;
		}
		allJson[sourceName] = sourceJson;
	}

	manager->deleteLater();

	QJsonDocument outDoc(allJson);
	QFile fOut(parser.value(outputOption));
	if (!fOut.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
		return 1;

	fOut.write(outDoc.toJson());
	fOut.close();

	return 0;
}
