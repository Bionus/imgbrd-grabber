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

	QCommandLineOption tagsOption(QStringList() << "t" << "tags", "Tags to search for.", "tags");
	QCommandLineOption sourceOption(QStringList() << "s" << "sources", "Source websites.", "sources");
	QCommandLineOption pageOption(QStringList() << "p" << "page", "Starting page.", "page", "1");
	QCommandLineOption limitOption(QStringList() << "m" << "max", "Maximum of returned images.", "count");
	QCommandLineOption perPageOption(QStringList() << "i" << "perpage", "Number of images per page.", "count", "20");
	QCommandLineOption pathOption(QStringList() << "l" << "location", "Location to save the results.", "path");
	QCommandLineOption filenameOption(QStringList() << "f" << "filename", "Filename to save the results.", "filename");
	QCommandLineOption userOption(QStringList() << "u" << "user", "Username to connect to the source.", "user");
	QCommandLineOption passwordOption(QStringList() << "w" << "password", "Password to connect to the source.", "password");
	QCommandLineOption blacklistOption(QStringList() << "b" << "blacklist", "Download blacklisted images.");
	QCommandLineOption postFilteringOption(QStringList() << "r" << "postfilter", "Filter results.", "filter");
	QCommandLineOption noDuplicatesOption(QStringList() << "n" << "no-duplicates", "Remove duplicates from results.");
	QCommandLineOption verboseOption(QStringList() << "d" << "debug", "Show debug messages.");
	QCommandLineOption tagsMinOption(QStringList() << "tm" << "tags-min", "Minimum count for tags to be returned.", "count", "0");
	QCommandLineOption tagsFormatOption(QStringList() << "tf" << "tags-format", "Format for returning tags.", "format", "%tag\t%count\t%type");
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
	QCommandLineOption returnCountOption(QStringList() << "rc" << "return-count", "Return total image count.");
	QCommandLineOption returnTagsOption(QStringList() << "rt" << "return-tags", "Return tags for a search.");
	QCommandLineOption returnPureTagsOption(QStringList() << "rp" << "return-pure-tags", "Return tags.");
	QCommandLineOption returnImagesOption(QStringList() << "ri" << "return-images", "Return images url.");
	QCommandLineOption downloadOption(QStringList() << "download", "Download found images.");
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
	QObject::connect(downloader, SIGNAL(quit()), qApp, SLOT(quit()));

	return app.exec();
}
