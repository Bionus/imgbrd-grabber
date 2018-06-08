#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QTimer>
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/page.h"
#include "models/page-api.h"
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

	Logger::getInstance().setLogLevel(Logger::Warning);

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

	QJsonObject root = input.object();
	QJsonArray rootSearch = root.value("search").toArray();
	QJsonObject sources = root.value("sources").toObject();

	for (const QString &sourceName : sources.keys())
	{
		qDebug() << "#" << "Source" << sourceName;
		QJsonObject sourceJson;

		Source *source = allSources.value(sourceName);
		QJsonObject sites = sources.value(sourceName).toObject();

		QJsonObject sourceApis = sites.value("apis").toObject();
		QJsonArray sourceSearch = rootSearch;
		if (sites.contains("search"))
		{ sourceSearch = sites.value("search").toArray(); }

		for (Site *site : source->getSites())
		{
			qDebug() << "##" << "Site" << site->url();
			QJsonObject siteJson;

			QJsonObject siteApis = sourceApis;
			QJsonArray siteSearch = sourceSearch;
			if (sites.contains(site->url()))
			{
				QJsonObject override = sites.value(site->url()).toObject();
				if (override.contains("apis"))
				{ siteApis = override.value("apis").toObject(); }
				if (override.contains("search"))
				{ siteSearch = override.value("search").toArray(); }
			}

			for (const QString &apiName : siteApis.keys())
			{
				qDebug() << "###" << "API" << apiName;
				QJsonObject apiJson;
				QJsonArray checks = siteApis.value(apiName).toArray();

				QJsonArray apiSearch = siteSearch;
				if (checks.count() > 4)
				{ apiSearch = checks[4].toArray(); }

				QString search = apiSearch[0].toString();
				int pagei = apiSearch[1].toDouble();
				int limit = apiSearch[2].toDouble();

				Api *api = Q_NULLPTR;
				for (Api *a : site->getApis())
					if (a->getName().toLower() == apiName.toLower())
						api = a;
				if (api == Q_NULLPTR)
					continue;

				auto page = new Page(profile, site, allSites.values(), QStringList() << search, pagei, limit);
				auto pageApi = new PageApi(page, profile, site, api, search.split(' '), pagei, limit);
				QEventLoop loop;
				QObject::connect(pageApi, &PageApi::finishedLoading, &loop, &QEventLoop::quit);
				QTimer::singleShot(1, pageApi, SLOT(load()));
				loop.exec();

				apiJson["status"] = "ok";
				QStringList message;

				// Checks
				if (!jsonCompare(pageApi->errors().join(", "), checks[0]))
				{
					apiJson["status"] = "error";
					message.append(pageApi->errors());
				}
				if (!jsonCompare(pageApi->imagesCount(false), checks[1]))
				{
					if (apiJson["status"] == "ok")
					{ apiJson["status"] = "warning"; }
					message.append("Image count error: " + QString::number(pageApi->imagesCount(false)));
				}
				if (!jsonCompare(pageApi->images().count(), checks[2]))
				{
					apiJson["status"] = "error";
					message.append("Number of images error: " + QString::number(pageApi->images().count()));
				}
				if (!jsonCompare(pageApi->tags().count(), checks[3]))
				{
					if (apiJson["status"] == "ok")
					{ apiJson["status"] = "warning"; }
					QStringList tags;
					for (const Tag& tag : pageApi->tags())
					{ tags.append(tag.text()); }
					message.append("Number of tags error: " + QString::number(pageApi->tags().count()) + " [" + tags.join(", ") + "]");
				}

				if (!message.isEmpty())
				{
					apiJson["message"] = message.join(", ");
					siteJson[apiName] = apiJson;
				}
				else
				{ siteJson[apiName] = apiJson["status"]; }

				pageApi->deleteLater();
				page->deleteLater();
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
