/**
 * @file
 * @author  Bionus <bio.nus@hotmail.fr>
 *
 * @section LICENSE
 *
 * Copyright 2011 - 2012 Bionus
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * Batch downloader and viewer for boorus/imageboards.
 */


#include <QApplication>
#include <QDir>
#include "downloader/downloader.h"
#include "functions.h"
#include "mainwindow.h"
#include "models/page-api.h"
#include "models/profile.h"
#include "models/site.h"
#include "updater/update-dialog.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	#include <QCommandLineParser>
#else
	#include <vendor/qcommandlineparser.h>
#endif
#if !defined(USE_CLI) && defined(USE_BREAKPAD)
	#include <QFileInfo>
	#include "crashhandler/crashhandler.h"
#endif



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	qRegisterMetaType<PageApi::LoadResult>("LoadResult");

	// Set window title according to the current build
	#ifdef NIGHTLY
		QString commit(NIGHTLY_COMMIT);
		if (!commit.isEmpty())
			app.setApplicationDisplayName("Grabber Nightly - " + commit.left(8));
		else
			app.setApplicationDisplayName("Grabber Nightly");
	#else
		app.setApplicationDisplayName("Grabber");
	#endif

	// Copy settings files to writable directory
	QStringList toCopy = QStringList() << "sites/" << "themes/" << "webservices/";
	for (const QString &tgt : toCopy)
	{
		QString from = savePath(tgt, true, false);
		QString to = savePath(tgt, true, true);
		if (!QDir(to).exists() && QDir(from).exists())
			copyRecursively(from, to);
	}

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	#if !defined(USE_CLI)
		QCommandLineOption cliOption(QStringList() << "c" << "cli", "Disable the GUI.");
		parser.addOption(cliOption);
	#endif
	QCommandLineOption tagsOption(QStringList() << "t" << "tags", "Tags to search for.", "tags");
	QCommandLineOption sourceOption(QStringList() << "s" << "sources", "Source websites.", "sources");
	QCommandLineOption pageOption(QStringList() << "p" << "page", "Starting page.", "page", "1");
	QCommandLineOption limitOption(QStringList() << "m" << "max", "Maximum of returned images.", "count");
	QCommandLineOption perpageOption(QStringList() << "i" << "perpage", "Number of images per page.", "count", "20");
	QCommandLineOption pathOption(QStringList() << "l" << "location", "Location to save the results.", "path");
	QCommandLineOption filenameOption(QStringList() << "f" << "filename", "Filename to save the results.", "filename");
	QCommandLineOption userOption(QStringList() << "u" << "user", "Username to connect to the source.", "user");
	QCommandLineOption passwordOption(QStringList() << "w" << "password", "Password to connect to the source.", "password");
	QCommandLineOption blacklistOption(QStringList() << "b" << "blacklist", "Download blacklisted images.");
	QCommandLineOption postfilteringOption(QStringList() << "r" << "postfilter", "Filter results.", "filter");
	QCommandLineOption noDuplicatesOption(QStringList() << "n" << "no-duplicates", "Remove duplicates from results.");
	QCommandLineOption verboseOption(QStringList() << "d" << "debug", "Show debug messages.");
	QCommandLineOption tagsMinOption(QStringList() << "tm" << "tags-min", "Minimum count for tags to be returned.", "count", "0");
	QCommandLineOption tagsFormatOption(QStringList() << "tf" << "tags-format", "Format for returning tags.", "format", "%tag\t%count\t%type");
	parser.addOption(tagsOption);
	parser.addOption(sourceOption);
	parser.addOption(pageOption);
	parser.addOption(limitOption);
	parser.addOption(perpageOption);
	parser.addOption(pathOption);
	parser.addOption(filenameOption);
	parser.addOption(userOption);
	parser.addOption(passwordOption);
	parser.addOption(blacklistOption);
	parser.addOption(postfilteringOption);
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

	#if !defined(USE_CLI)
		bool gui = !parser.isSet(cliOption);
	#else
		bool gui = false;
	#endif

	bool verbose = parser.isSet(verboseOption);
	#if !defined(QT_DEBUG)
		Logger::setupMessageOutput(gui || verbose);
	#endif
	if (verbose)
		Logger::getInstance().setLogLevel(Logger::Debug);

	#if defined(USE_BREAKPAD) && !defined(USE_CLI)
		if (gui)
		{
			QDir dir = QFileInfo(argv[0]).dir();
			QString crashes = savePath("crashes");
			if (!dir.exists(crashes))
			{ dir.mkpath(crashes); }
			CrashHandler::instance()->Init(crashes);
		}
	#endif

	Profile *profile = new Profile(savePath());

	if (!gui)
	{
		Downloader *dwnldr = new Downloader(profile,
											parser.value(tagsOption).split(" ", QString::SkipEmptyParts),
											parser.value(postfilteringOption).split(" ", QString::SkipEmptyParts),
											profile->getFilteredSites(parser.value(sourceOption).split(" ", QString::SkipEmptyParts)),
											parser.value(pageOption).toInt(),
											parser.value(limitOption).toInt(),
											parser.value(perpageOption).toInt(),
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
			dwnldr->getPageCount();
		else if (parser.isSet(returnTagsOption))
			dwnldr->getPageTags();
		else if (parser.isSet(returnPureTagsOption))
			dwnldr->getTags();
		else if (parser.isSet(returnImagesOption))
			dwnldr->getUrls();
		else if (parser.isSet(downloadOption))
			dwnldr->getImages();
		else
			parser.showHelp();

		dwnldr->setQuit(true);
		QObject::connect(dwnldr, &Downloader::quit, qApp, &QApplication::quit);
	}
	#if !defined(USE_CLI)
		else
		{
			// Check for updates
			QSettings *settings = profile->getSettings();
			int cfuInterval = settings->value("check_for_updates", 24*60*60).toInt();
			QDateTime lastCfu = settings->value("last_check_for_updates", QDateTime()).toDateTime();
			if (cfuInterval >= 0 && (!lastCfu.isValid() || lastCfu.addSecs(cfuInterval) <= QDateTime::currentDateTime()))
			{
				settings->setValue("last_check_for_updates", QDateTime::currentDateTime());

				bool shouldQuit = false;
				auto *updateDialog = new UpdateDialog(&shouldQuit);
				auto *el = new QEventLoop();
				QObject::connect(updateDialog, &UpdateDialog::noUpdateAvailable, el, &QEventLoop::quit);
				QObject::connect(updateDialog, &UpdateDialog::rejected, el, &QEventLoop::quit);

				updateDialog->checkForUpdates();
				el->exec();
				el->deleteLater();

				if (shouldQuit)
					return 0;
			}

			QMap<QString, QString> params;
			params.insert("booru", parser.value(sourceOption));
			params.insert("limit", parser.value(limitOption));
			params.insert("page", parser.value(pageOption));
			params.insert("path", parser.value(pathOption));
			params.insert("filename", parser.value(filenameOption));
			params.insert("user", parser.value(userOption));
			params.insert("password", parser.value(passwordOption));
			params.insert("ignore", parser.isSet(blacklistOption) ? "true" : "false");
			params.insert("tags", parser.value(tagsOption));

			auto *mainwindow = new mainWindow(profile);
			mainwindow->init(parser.positionalArguments(), params);
			mainwindow->show();
		}
	#endif

	return app.exec();
}
