#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QTimer>
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/page.h"
#include "models/page-api.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag.h"


bool opCompare(const QString &op, int left, int right)
{
	if (right == -1) {
		return true;
	}
	if (op == ">") {
		return left > right;
	}
	if (op == "<") {
		return left < right;
	}
	return left == right;
}

bool jsonCompare(const QVariant &value, QJsonValue opt)
{
	QString op = "=";
	if (opt.isArray()) {
		QJsonArray arrOpt = opt.toArray();
		op = arrOpt[0].toString();
		opt = arrOpt[1];
	}

	if (value.type() == QVariant::String) {
		return value.toString() == opt.toString();
	}

	return opCompare(op, value.toInt(), opt.toInt());
}

int main(int argc, char *argv[])
{
	const QCoreApplication app(argc, argv);

	QCommandLineParser parser;
	parser.addHelpOption();

	const QCommandLineOption inputOption(QStringList() << "i" << "input", "Input JSON configuration file", "input");
	const QCommandLineOption outputOption(QStringList() << "o" << "output", "Output JSON result file", "output");
	parser.addOption(inputOption);
	parser.addOption(outputOption);
	parser.process(app);

	Logger::getInstance().setLogLevel(Logger::Warning);

	QFile f(parser.value(inputOption));
	if (!f.open(QFile::ReadOnly | QFile::Text)) {
		return 1;
	}

	QJsonObject allJson;
	QJsonDocument input = QJsonDocument::fromJson(f.readAll());
	f.close();

	Profile *profile = new Profile(savePath());
	auto allSources = profile->getSources();
	auto allSites = profile->getSites();

	const auto oldBlacklist = profile->getBlacklist();
	profile->setBlacklistedTags(Blacklist());

	const QJsonObject root = input.object();
	const QJsonArray rootSearch = root.value("search").toArray();
	const QJsonObject sources = root.value("sources").toObject();

	for (auto it = sources.constBegin(); it != sources.constEnd(); ++it) {
		const QString &sourceName = it.key();
		qDebug() << "#" << "Source" << sourceName;
		QJsonObject sourceJson;

		Source *source = allSources.value(sourceName);
		QJsonObject sites = it.value().toObject();

		const QJsonObject sourceApis = sites.value("apis").toObject();
		QJsonArray sourceSearch = rootSearch;
		if (sites.contains("search")) {
			sourceSearch = sites.value("search").toArray();
		}

		for (Site *site : source->getSites()) {
			qDebug() << "##" << "Site" << site->url();
			QJsonObject siteJson;

			QJsonObject siteApis = sourceApis;
			QJsonArray siteSearch = sourceSearch;
			if (sites.contains(site->url())) {
				QJsonObject override = sites.value(site->url()).toObject();
				if (override.contains("apis")) {
					siteApis = override.value("apis").toObject();
				}
				if (override.contains("search")) {
					siteSearch = override.value("search").toArray();
				}
			}

			for (auto ita = siteApis.constBegin(); ita != siteApis.constEnd(); ++ita) {
				const QString &apiName = ita.key();
				qDebug() << "###" << "API" << apiName;
				QJsonObject apiJson;
				QJsonArray checks = ita.value().toArray();

				QJsonArray apiSearch = siteSearch;
				if (checks.count() > 4) {
					apiSearch = checks[4].toArray();
				}

				const QString search = apiSearch[0].toString();
				const int pageI = apiSearch[1].toInt();
				const int limit = apiSearch[2].toInt();

				Api *api = nullptr;
				for (Api *a : site->getApis()) {
					if (a->getName().toLower() == apiName.toLower()) {
						api = a;
					}
				}
				if (api == nullptr) {
					continue;
				}

				auto page = new Page(profile, site, allSites.values(), QStringList() << search, pageI, limit);
				auto pageApi = new PageApi(page, profile, site, api, search.split(' '), pageI, limit);
				QEventLoop loop;
				QObject::connect(pageApi, &PageApi::finishedLoading, &loop, &QEventLoop::quit);
				QTimer::singleShot(1, pageApi, SLOT(load()));
				loop.exec();

				apiJson["status"] = "ok";
				QStringList message;

				// Checks
				if (!jsonCompare(pageApi->errors().join(", "), checks[0])) {
					apiJson["status"] = "error";
					message.append(pageApi->errors());
				}
				if (!jsonCompare(pageApi->imagesCount(false), checks[1])) {
					if (apiJson["status"] == "ok") {
						apiJson["status"] = "warning";
					}
					message.append("Image count error: " + QString::number(pageApi->imagesCount(false)));
				}
				if (!jsonCompare(pageApi->images().count(), checks[2])) {
					apiJson["status"] = "error";
					message.append("Number of images error: " + QString::number(pageApi->images().count()));
				}
				if (!jsonCompare(pageApi->tags().count(), checks[3])) {
					if (apiJson["status"] == "ok") {
						apiJson["status"] = "warning";
					}
					QStringList tags;
					for (const Tag& tag : pageApi->tags()) {
						tags.append(tag.text());
					}
					message.append("Number of tags error: " + QString::number(pageApi->tags().count()) + " [" + tags.join(", ") + "]");
				}

				if (!message.isEmpty()) {
					apiJson["message"] = message.join(", ");
					siteJson[apiName] = apiJson;
				} else {
					siteJson[apiName] = apiJson["status"];
				}

				pageApi->deleteLater();
				page->deleteLater();
			}
			sourceJson[site->url()] = siteJson;
		}
		allJson[sourceName] = sourceJson;
	}

	profile->setBlacklistedTags(oldBlacklist);

	QJsonDocument outDoc(allJson);
	QFile fOut(parser.value(outputOption));
	if (!fOut.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
		return 1;
	}

	fOut.write(outDoc.toJson());
	fOut.close();

	return 0;
}
