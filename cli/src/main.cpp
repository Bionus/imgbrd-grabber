#include <QCoreApplication>
#include "downloader/downloader.h"
#include "functions.h"
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
	const QCommandLineOption postFilteringOption(QStringList() << "r" << "postfilter", "Filter results.", "filter");
	const QCommandLineOption noDuplicatesOption(QStringList() << "n" << "no-duplicates", "Remove duplicates from results.");
	const QCommandLineOption verboseOption(QStringList() << "d" << "debug", "Show debug messages.");
	const QCommandLineOption tagsMinOption(QStringList() << "tm" << "tags-min", "Minimum count for tags to be returned.", "count", "0");
	const QCommandLineOption tagsFormatOption(QStringList() << "tf" << "tags-format", "Format for returning tags.", "format", "%tag\t%count\t%type");
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
	parser.addOption(postFilteringOption);
	parser.addOption(tagsMinOption);
	parser.addOption(tagsFormatOption);
	parser.addOption(noDuplicatesOption);
	parser.addOption(verboseOption);
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

	#ifndef QT_DEBUG
		Logger::setupMessageOutput(parser.isSet(verboseOption));
	#endif

	Profile *profile = new Profile(savePath());
	profile->purgeTemp(24 * 60 * 60);

	Downloader *downloader = new Downloader(profile,
		parser.value(tagsOption).split(" ", QString::SkipEmptyParts),
		parser.value(postFilteringOption).split(" ", QString::SkipEmptyParts),
		profile->getFilteredSites(parser.value(sourceOption).split(" ", QString::SkipEmptyParts)),
		parser.value(pageOption).toInt(),
		parser.value(limitOption).toInt(),
		parser.value(perPageOption).toInt(),
		parser.value(pathOption),
		parser.value(filenameOption),
		parser.value(userOption),
		parser.value(passwordOption),
		parser.isSet(blacklistOption),
		profile->getBlacklist(),
		parser.isSet(noDuplicatesOption),
		parser.value(tagsMinOption).toInt(),
		parser.value(tagsFormatOption));

	if (parser.isSet(returnCountOption))
		downloader->getPageCount();
	else if (parser.isSet(returnTagsOption))
		downloader->getPageTags();
	else if (parser.isSet(returnPureTagsOption))
		downloader->getTags();
	else if (parser.isSet(returnImagesOption))
		downloader->getUrls();
	else if (parser.isSet(downloadOption))
		downloader->getImages();
	else
		parser.showHelp();

	downloader->setQuit(true);
	QObject::connect(downloader, &Downloader::quit, qApp, &QCoreApplication::quit);

	return app.exec();
}
