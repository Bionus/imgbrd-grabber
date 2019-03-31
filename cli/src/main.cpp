#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkProxy>
#include <QUrl>
#include "downloader/downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/filtering/blacklist.h"
#include "models/profile.h"
#include "models/site.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	#include <QCommandLineParser>
#else
	#include <vendor/qcommandlineparser.h>
#endif

void loadMoreDetails(const QList<QSharedPointer<Image>> &images);
QJsonObject serializeImg(const Image *image, const QMap<QString, Token>& tokens);
void writeToFile(QString filename, QByteArray data);

void returnJsonArray(const QJsonArray &array)
{
	QJsonDocument jsonDoc;
	jsonDoc.setArray(array);

	QByteArray jsonResult = jsonDoc.toJson(QJsonDocument::Indented);
	QTextStream(stdout) << qPrintable(jsonResult);

	qApp->quit();
}

void serializeTags(const QList<Tag> &tags)
{
	QJsonArray jsonArray;
	for (const Tag &tag : tags) {
		QJsonObject jsonObj;
		tag.write(jsonObj);
		jsonArray.append(jsonObj);
	}
	returnJsonArray(jsonArray);
}

void serializeImages(Profile *profile, const QList<QSharedPointer<Image>> &images)
{
	QJsonArray jsonArray;
	for (const auto &image : images) {
		auto tokens = image->tokens(profile);
		auto jsObject = serializeImg(image.data(), tokens);
		jsonArray.append(jsObject);
	}
	returnJsonArray(jsonArray);
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	const QCommandLineOption tagsOption(QStringList() << "t" << "tags", "Tags to search for.", "tags");
	const QCommandLineOption sourceOption(QStringList() << "s" << "sources", "Source websites.", "sources");
	const QCommandLineOption pageOption(QStringList() << "p" << "page", "Starting page.", "page", "1");
	const QCommandLineOption limitOption(QStringList() << "m" << "max", "Maximum of returned images.", "count");
	const QCommandLineOption perPageOption(QStringList() << "i" << "perpage", "Number of images per page.", "count", "20");
	const QCommandLineOption pathOption(QStringList() << "l" << "location", "Location to save the results.", "path");
	const QCommandLineOption filenameOption(QStringList() << "f" << "filename", "Filename to save the results.", "filename");
	const QCommandLineOption userOption(QStringList() << "u" << "user", "Username to connect to the source.", "user");
	const QCommandLineOption passwordOption(QStringList() << "w" << "password", "Password to connect to the source.", "password");
	const QCommandLineOption blacklistOption(QStringList() << "b" << "blacklist", "Download blacklisted images.");
	const QCommandLineOption tagsBlacklistOption(QStringList() << "tb" << "tags-blacklist" , "Tags to remove from results.", "tags-blacklist");
	const QCommandLineOption postFilteringOption(QStringList() << "r" << "postfilter", "Filter results.", "filter");
	const QCommandLineOption noDuplicatesOption(QStringList() << "n" << "no-duplicates", "Remove duplicates from results.");
	const QCommandLineOption verboseOption(QStringList() << "d" << "debug", "Show debug messages.");
	const QCommandLineOption tagsMinOption(QStringList() << "tm" << "tags-min", "Minimum count for tags to be returned.", "count", "0");
	const QCommandLineOption tagsFormatOption(QStringList() << "tf" << "tags-format", "Format for returning tags.", "format", "%tag\t%count\t%type");
	const QCommandLineOption ignoreErrorOption(QStringList() << "ignore-error", "don't exit on error.");
	const QCommandLineOption proxyOption(QStringList() << "proxy", "Use given proxy.", "[user:password]@host:port", "");
	const QCommandLineOption noLoginOption(QStringList() << "no-login", "disable auto login.");
	const QCommandLineOption jsonOption(QStringList() << "j" << "json", "output results as json.");
	const QCommandLineOption loadDetailsOption(QStringList() << "load-details", "request (more) details on found items.");
	const QCommandLineOption getDetailsOption(QStringList() << "get-details", "parse details from given link.", "url-page");
	parser.addOption(tagsOption);
	parser.addOption(sourceOption);
	parser.addOption(pageOption);
	parser.addOption(limitOption);
	parser.addOption(perPageOption);
	parser.addOption(pathOption);
	parser.addOption(filenameOption);
	parser.addOption(userOption);
	parser.addOption(passwordOption);
	parser.addOption(blacklistOption);
	parser.addOption(tagsBlacklistOption);
	parser.addOption(postFilteringOption);
	parser.addOption(tagsMinOption);
	parser.addOption(tagsFormatOption);
	parser.addOption(noDuplicatesOption);
	parser.addOption(verboseOption);
	parser.addOption(ignoreErrorOption);
	parser.addOption(proxyOption);
	parser.addOption(noLoginOption);
	parser.addOption(jsonOption);
	parser.addOption(loadDetailsOption);
	parser.addOption(getDetailsOption);
	const QCommandLineOption returnCountOption(QStringList() << "rc" << "return-count", "Return total image count.");
	const QCommandLineOption returnTagsOption(QStringList() << "rt" << "return-tags", "Return tags for a search.");
	const QCommandLineOption returnPureTagsOption(QStringList() << "rp" << "return-pure-tags", "Return tags.");
	const QCommandLineOption returnImagesOption(QStringList() << "ri" << "return-images", "Return images url.");
	const QCommandLineOption downloadOption(QStringList() << "download", "Download found images.");
	parser.addOption(returnCountOption);
	parser.addOption(returnTagsOption);
	parser.addOption(returnPureTagsOption);
	parser.addOption(returnImagesOption);
	parser.addOption(downloadOption);

	parser.process(app);

	if (parser.isSet(verboseOption)) {
		#ifndef QT_DEBUG
			Logger::getInstance().logToConsole();
		#endif
		Logger::getInstance().setLogLevel(Logger::Debug);
	}

	if (!parser.isSet(ignoreErrorOption)) {
		Logger::getInstance().setExitOnError(true);
	}

	if (parser.isSet(proxyOption)) {
		QUrl proxyUrl = QUrl::fromUserInput(parser.value(proxyOption));

		if (!proxyUrl.isValid()) {
			log(proxyUrl.errorString(), Logger::Error);
		}
		if (proxyUrl.port() == -1) {
			log("Bad proxy port.", Logger::Error);
		}

		const auto type = proxyUrl.scheme().startsWith("socks")
		                  ? QNetworkProxy::Socks5Proxy
		                  : QNetworkProxy::HttpProxy;

		const QNetworkProxy proxy(
		        type,
		        proxyUrl.host(),
		        proxyUrl.port(),
		        proxyUrl.userName(),
		        proxyUrl.password()
		    );

		QNetworkProxy::setApplicationProxy(proxy);
		log(QStringLiteral("Enabling application proxy on host \"%1\" and port %2.").arg(proxyUrl.host()).arg(proxyUrl.port()), Logger::Info);
	}

	Profile *profile = new Profile(savePath());
	profile->purgeTemp(24 * 60 * 60);

	auto sites = profile->getFilteredSites(parser.value(sourceOption).split(" ", QString::SkipEmptyParts));
	if (parser.isSet(noLoginOption)) {
		for (auto& site : sites) {
			site->setAutoLogin(false);
		}
	}

	if (parser.isSet(getDetailsOption)) {
		if (sites.length() != 1) {
			throw std::runtime_error("number of provided sites must be 1");
		}
		if (!parser.isSet(verboseOption)) {
			Logger::getInstance().setLogLevel(Logger::Error);
		}

		QString detailsUrl = parser.value(getDetailsOption);
		QMap<QString, QString> details = {{"page_url", detailsUrl}};
		Image image(sites[0], details, profile);
		image.setPromoteDetailParsWarn(true);

		QEventLoop loop;
		image.loadDetails();
		QObject::connect(&image, &Image::finishedLoadingTags, &loop, &QEventLoop::quit);
		loop.exec();

		auto tokens = image.tokens(profile);
		auto jsObject = serializeImg(&image, tokens);

		QJsonDocument jsonDoc;
		jsonDoc.setObject(jsObject);
		auto jsonResult = jsonDoc.toJson(QJsonDocument::Indented);
		QTextStream(stdout) << qPrintable(jsonResult);

		exit(0);
	}

	QString blacklistOverride = parser.value(tagsBlacklistOption);
	Downloader *downloader = new Downloader(profile,
		parser.value(tagsOption).split(" ", QString::SkipEmptyParts),
		parser.value(postFilteringOption).split(" ", QString::SkipEmptyParts),
		sites,
		parser.value(pageOption).toInt(),
		parser.value(limitOption).toInt(),
		parser.value(perPageOption).toInt(),
		parser.value(pathOption),
		parser.value(filenameOption),
		parser.value(userOption),
		parser.value(passwordOption),
		parser.isSet(blacklistOption),
		blacklistOverride.isEmpty() ? profile->getBlacklist() : Blacklist(blacklistOverride.split(' ')),
		parser.isSet(noDuplicatesOption),
		parser.value(tagsMinOption).toInt(),
		parser.value(tagsFormatOption));

	downloader->setQuit(true);

	// JSON output
	if (parser.isSet(jsonOption)) {
		downloader->setQuit(false);

		QObject::connect(downloader, &Downloader::finishedTags, serializeTags);
		QObject::connect(downloader, &Downloader::finishedImages, [&](const QList<QSharedPointer<Image>> &images) {
			if (parser.isSet(loadDetailsOption)) {
				loadMoreDetails(images);
			}
			serializeImages(profile, images);
		});
	}

	// Load the correct data
	if (parser.isSet(returnCountOption)) {
		downloader->getPageCount();
	} else if (parser.isSet(returnTagsOption)) {
		downloader->getPageTags();
	} else if (parser.isSet(returnPureTagsOption)) {
		downloader->getTags();
	} else if (parser.isSet(returnImagesOption)) {
		downloader->getUrls();
	} else if (parser.isSet(downloadOption) || parser.isSet(jsonOption)) {
		downloader->getImages();
	} else {
		parser.showHelp();
	}

	QObject::connect(downloader, &Downloader::quit, qApp, &QCoreApplication::quit);

	return app.exec();
}

void loadMoreDetails(const QList<QSharedPointer<Image>> &images)
{
	int work = images.length();
	QEventLoop loop;
	int requestsLimit = 5;  // simultan requests
	int runningRequests = 0;
	for (auto& image : images) {
		while (runningRequests >= requestsLimit) {
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		}
		runningRequests++;
		image->loadDetails();
		QObject::connect(image.data(), &Image::finishedLoadingTags, [&](){
			work--;
			runningRequests--;
			if (!work) {
				loop.quit();
			}
		});
	}
	loop.exec();
}

QJsonObject serializeImg(const Image *image, const QMap<QString, Token> &tokens)
{
	static QStringList ignoreKeys = {"all", "allo", "allos", "all_namespaces", };
	QJsonObject jsObject;

	for (auto& key : tokens.keys()) {
		typedef QVariant::Type Type;
		if (ignoreKeys.contains(key)) {
			continue;
		}
		if (key.contains("search_")) {
			continue;
		}

		const QVariant& qvalue = tokens.value(key).value();
		auto type = qvalue.type();

		if (type == QVariant::Type::StringList) {
			QStringList l = qvalue.toStringList();
			if (l.isEmpty()) {
				continue;
			}
			jsObject.insert(key, QJsonArray::fromStringList(l));
		} else if (type == QVariant::Type::String) {
			QString s = qvalue.toString();
			if (s.isEmpty()) {
				continue;
			}
			jsObject.insert(key, s);
		} else if (type == Type::Url || type == Type::ULongLong || type == Type::LongLong) {
			jsObject.insert(key, qvalue.toString());
		} else if (type == Type::Int) {
			jsObject.insert(key, qvalue.value<int>());
		} else if (type == Type::Bool) {
			jsObject.insert(key, qvalue.value<bool>());
		} else if (type == Type::DateTime) {
			jsObject.insert(key, static_cast<int>(qvalue.value<QDateTime>().toTime_t()));
		} else {
			qDebug() << qvalue;
			log(QStringLiteral("using generic QVariant::toString for key: %1").arg(key), Logger::Warning);
			jsObject.insert(key, qvalue.toString());
		}
	}
	jsObject.insert("isVideo", image->isVideo());
	jsObject.insert("isGallery", image->isGallery());
	jsObject.insert("isAnimated", image->isAnimated());
	return jsObject;
}

void writeToFile(QString filename, QByteArray data)
{
	QFile file(filename);
	file.open(QFile::WriteOnly);
	file.write(data);
	file.close();
}
