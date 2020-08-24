#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkProxy>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <stdexcept>
#include "downloader/downloader.h"
#include "downloader/printers/json-printer.h"
#include "downloader/printers/simple-printer.h"
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


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	Profile *profile = new Profile(savePath());
	profile->purgeTemp(24 * 60 * 60);

	QSettings *settings = profile->getSettings();
	QString dPath = settings->value("Save/path", "").toString();
	QString dFilename = settings->value("Save/filename", "").toString();

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	const QCommandLineOption tagsOption(QStringList() << "t" << "tags", "Tags to search for.", "tags");
	const QCommandLineOption sourceOption(QStringList() << "s" << "sources", "Source websites.", "sources");
	const QCommandLineOption pageOption(QStringList() << "p" << "page", "Starting page.", "page", "1");
	const QCommandLineOption limitOption(QStringList() << "m" << "max", "Maximum of returned images.", "count");
	const QCommandLineOption perPageOption(QStringList() << "i" << "perpage", "Number of images per page.", "count", "20");
	const QCommandLineOption pathOption(QStringList() << "l" << "location", "Location to save the results.", "path", dPath);
	const QCommandLineOption filenameOption(QStringList() << "f" << "filename", "Filename to save the results.", "filename", dFilename);
	const QCommandLineOption userOption(QStringList() << "u" << "user", "Username to connect to the source.", "user");
	const QCommandLineOption passwordOption(QStringList() << "w" << "password", "Password to connect to the source.", "password");
	const QCommandLineOption blacklistOption(QStringList() << "b" << "blacklist", "Download blacklisted images.");
	const QCommandLineOption tagsBlacklistOption(QStringList() << "tb" << "tags-blacklist", "Tags to remove from results.", "tags-blacklist");
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
	const QCommandLineOption returnImagesOption(QStringList() << "ri" << "return-images", "Return images.");
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

	auto sites = profile->getFilteredSites(parser.value(sourceOption).split(" ", QString::SkipEmptyParts));
	if (parser.isSet(noLoginOption)) {
		for (auto& site : sites) {
			site->setAutoLogin(false);
		}
	}

	Printer *printer = parser.isSet(jsonOption)
		? (Printer*) new JsonPrinter(profile)
		: (Printer*) new SimplePrinter(parser.value(tagsFormatOption));

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

		printer->print(image);
		exit(0);
	}

	if (parser.value(filenameOption).isEmpty() && parser.isSet(downloadOption)) {
		QTextStream(stderr) << "You need a filename for downloading images";
		exit(1);
	}

	QString blacklistOverride = parser.value(tagsBlacklistOption);
	Downloader *downloader = new Downloader(profile, printer,
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
		parser.isSet(loadDetailsOption),
		!parser.isSet(noLoginOption));

	downloader->setQuit(true);

	// Load the correct data
	if (parser.isSet(returnCountOption)) {
		downloader->getPageCount();
	} else if (parser.isSet(returnTagsOption)) {
		downloader->getPageTags();
	} else if (parser.isSet(returnPureTagsOption)) {
		downloader->getTags();
	} else if (parser.isSet(returnImagesOption)) {
		downloader->getUrls();
	} else if (parser.isSet(downloadOption)) {
		downloader->getImages();
	} else {
		parser.showHelp();
	}

	QObject::connect(downloader, &Downloader::quit, qApp, &QCoreApplication::quit);

	return app.exec();
}
