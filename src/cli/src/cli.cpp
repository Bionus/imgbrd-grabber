#include <QCommandLineParser>
#include <QNetworkProxy>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include "cli.h"
#include "cli-commands/download-images-cli-command.h"
#include "cli-commands/get-details-cli-command.h"
#include "cli-commands/get-images-cli-command.h"
#include "cli-commands/get-page-count-cli-command.h"
#include "cli-commands/get-page-tags-cli-command.h"
#include "cli-commands/get-tags-cli-command.h"
#include "cli-commands/load-tag-database-cli-command.h"
#include "printers/json-printer.h"
#include "printers/simple-printer.h"
#include "logger.h"
#include "models/filtering/blacklist.h"
#include "models/profile.h"
#include "models/site.h"


int parseAndRunCliArgs(QCoreApplication *app, Profile *profile, bool defaultToGui, QMap<QString, QString> &params, QStringList &positionalArgs)
{
	QSettings *settings = profile->getSettings();
	QString dPath = settings->value("Save/path", "").toString();
	QString dFilename = settings->value("Save/filename", "").toString();

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	const QCommandLineOption cliOption(QStringList() << "c" << "cli", "Disable the GUI.");
	if (defaultToGui) {
		parser.addOption(cliOption);
	}

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
	const QCommandLineOption loadTagDatabaseOption(QStringList() << "load-tag-database", "load the tag database of the given sources.");
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
	parser.addOption(loadTagDatabaseOption);
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

	parser.process(*app);

	const bool gui = defaultToGui && !parser.isSet(cliOption);

	// Log messages output and level
	const bool verbose = parser.isSet(verboseOption);
	Logger::setupMessageOutput(gui || verbose);
	Logger::getInstance().setLogLevel(verbose ? Logger::Debug : Logger::Info);
	Logger::getInstance().setConsoleOutputLevel(verbose ? Logger::Debug : Logger::Error);

	// Stop here for GUI, but pass some information to the main window from the parser later
	if (gui) {
		// TODO(Bionus): get rid of this
		params.insert("booru", parser.value(sourceOption));
		params.insert("limit", parser.value(limitOption));
		params.insert("page", parser.value(pageOption));
		params.insert("path", parser.value(pathOption));
		params.insert("filename", parser.value(filenameOption));
		params.insert("user", parser.value(userOption));
		params.insert("password", parser.value(passwordOption));
		params.insert("ignore", parser.isSet(blacklistOption) ? "true" : "false");
		params.insert("tags", parser.value(tagsOption));

		positionalArgs.append(parser.positionalArguments());

		return -1;
	}

	// Generate a runtime error when an error log arrives
	if (!parser.isSet(ignoreErrorOption)) {
		Logger::getInstance().setExitOnError(true);
	}

	// Proxy settings
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

	auto sites = profile->getFilteredSites(parser.value(sourceOption).split(" ", Qt::SkipEmptyParts));
	if (parser.isSet(noLoginOption)) {
		for (auto& site : sites) {
			site->setAutoLogin(false);
		}
	}

	QTextStream stream(stdout);
	Printer *printer = parser.isSet(jsonOption)
		? static_cast<Printer*>(new JsonPrinter(&stream, profile))
		: static_cast<Printer*>(new SimplePrinter(&stream, parser.value(tagsFormatOption)));

	CliCommand *cmd = nullptr;

	if (parser.isSet(loadTagDatabaseOption)) {
		int minTagCount = parser.value(tagsMinOption).toInt();
		minTagCount = minTagCount == 0 ? 10000 : minTagCount;

		cmd = new LoadTagDatabaseCliCommand(profile, sites, minTagCount);
	} else if (parser.isSet(getDetailsOption)) {
		const QString detailsUrl = parser.value(getDetailsOption);

		cmd = new GetDetailsCliCommand(profile, printer, sites, detailsUrl);
	} else if (parser.isSet(returnCountOption)) {
		const QStringList tags = parser.value(tagsOption).split(" ", Qt::SkipEmptyParts);
		const QStringList postFiltering = parser.value(postFilteringOption).split(" ", Qt::SkipEmptyParts);
		const int page = parser.value(pageOption).toInt();
		const int perPage = parser.value(perPageOption).toInt();

		cmd = new GetPageCountCliCommand(profile, printer, tags, postFiltering, sites, page, perPage);
	} else if (parser.isSet(returnTagsOption)) {
		const QStringList tags = parser.value(tagsOption).split(" ", Qt::SkipEmptyParts);
		const QStringList postFiltering = parser.value(postFilteringOption).split(" ", Qt::SkipEmptyParts);
		const int page = parser.value(pageOption).toInt();
		const int perPage = parser.value(perPageOption).toInt();
		const int tagsMin = parser.value(tagsMinOption).toInt();

		cmd = new GetPageTagsCliCommand(profile, printer, tags, postFiltering, sites, page, perPage, tagsMin);
	} else if (parser.isSet(returnPureTagsOption)) {
		const int page = parser.value(pageOption).toInt();
		const int perPage = parser.value(perPageOption).toInt();
		const int max = parser.value(limitOption).toInt();
		const int tagsMin = parser.value(tagsMinOption).toInt();

		cmd = new GetTagsCliCommand(profile, printer, sites, page, perPage, max, tagsMin);
	} else if (parser.isSet(returnImagesOption)) {
		const QStringList tags = parser.value(tagsOption).split(" ", Qt::SkipEmptyParts);
		const QStringList postFiltering = parser.value(postFilteringOption).split(" ", Qt::SkipEmptyParts);
		const int page = parser.value(pageOption).toInt();
		const int perPage = parser.value(perPageOption).toInt();
		const QString filename = parser.value(filenameOption);
		const QString folder = parser.value(pathOption);
		const int max = parser.value(limitOption).toInt();
		const bool login = !parser.isSet(noLoginOption);
		const bool noDuplicates = parser.isSet(noDuplicatesOption);
		const bool getBlacklisted = parser.isSet(blacklistOption);
		const bool loadMoreDetails = parser.isSet(loadDetailsOption);

		cmd = new GetImagesCliCommand(profile, printer, tags, postFiltering, sites, page, perPage, filename, folder, max, login, noDuplicates, getBlacklisted, loadMoreDetails);
	} else if (parser.isSet(downloadOption)) {
		const QStringList tags = parser.value(tagsOption).split(" ", Qt::SkipEmptyParts);
		const QStringList postFiltering = parser.value(postFilteringOption).split(" ", Qt::SkipEmptyParts);
		const int page = parser.value(pageOption).toInt();
		const int perPage = parser.value(perPageOption).toInt();
		const QString filename = parser.value(filenameOption);
		const QString folder = parser.value(pathOption);
		const int max = parser.value(limitOption).toInt();
		const bool login = !parser.isSet(noLoginOption);
		const bool noDuplicates = parser.isSet(noDuplicatesOption);
		const bool getBlacklisted = parser.isSet(blacklistOption);

		const QString blacklistOverride = parser.value(tagsBlacklistOption);
		const Blacklist blacklist = blacklistOverride.isEmpty() ? profile->getBlacklist() : Blacklist(blacklistOverride.split(' '));

		cmd = new DownloadImagesCliCommand(profile, printer, tags, postFiltering, sites, page, perPage, filename, folder, max, login, noDuplicates, getBlacklisted, blacklist);
	}

	if (cmd == nullptr || !cmd->validate()) {
		parser.showHelp();
		return 1;
	}

	QEventLoop loop;
	QObject::connect(cmd, &CliCommand::finished, &loop, &QEventLoop::quit);
	QTimer::singleShot(0, [cmd]() { cmd->run(); });
	loop.exec();

	cmd->deleteLater();
	return 0;
}
