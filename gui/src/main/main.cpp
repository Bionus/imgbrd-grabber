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
#include <QSettings>
#include "analytics.h"
#include "downloader/downloader.h"
#include "functions.h"
#include "logger.h"
#include "main-window.h"
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
#if defined(Q_OS_ANDROID)
	#include <QMessageBox>
	#include "android.h"
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
		if (!commit.isEmpty()) {
			app.setApplicationDisplayName("Grabber Nightly - " + commit.left(8));
			app.setApplicationVersion(QString(VERSION) + " - nightly " + commit.left(8));
		} else {
			app.setApplicationDisplayName("Grabber Nightly");
			app.setApplicationVersion(QString(VERSION) + " - nightly");
		}
	#else
		app.setApplicationDisplayName("Grabber");
	#endif

	// Copy settings files to writable directory
	QStringList toCopy = QStringList() << "sites/" << "themes/" << "webservices/";
	for (const QString &tgt : toCopy) {
		const QString from = savePath(tgt, true, false);
		const QString to = savePath(tgt, true, true);
		if (!QDir(to).exists() && QDir(from).exists()) {
			copyRecursively(from, to);
		}
	}

	#if defined(Q_OS_ANDROID)
		if (!checkPermission("android.permission.WRITE_EXTERNAL_STORAGE")) {
			QMessageBox::critical(nullptr, "Permission error", "Grabber needs storage writing permissions to download images");
			return 0;
		}
	#endif

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	#if !defined(USE_CLI)
		const QCommandLineOption cliOption(QStringList() << "c" << "cli", "Disable the GUI.");
		parser.addOption(cliOption);
	#endif
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

	#if !defined(USE_CLI)
		const bool gui = !parser.isSet(cliOption);
	#else
		const bool gui = false;
	#endif

	const bool verbose = parser.isSet(verboseOption);
	#if !defined(QT_DEBUG)
		Logger::setupMessageOutput(gui || verbose);
	#endif
	if (verbose) {
		Logger::getInstance().setLogLevel(Logger::Debug);
	}

	#if defined(USE_BREAKPAD) && !defined(USE_CLI)
		if (gui) {
			QDir dir = QFileInfo(argv[0]).dir();
			QString crashes = savePath("crashes");
			if (!dir.exists(crashes)) {
				dir.mkpath(crashes);
			}
			CrashHandler::instance()->Init(crashes);
		}
	#endif

	Profile *profile = new Profile(savePath());
	profile->purgeTemp(24 * 60 * 60);
	QSettings *settings = profile->getSettings();

	// Analytics
	Analytics::getInstance().setTrackingID("UA-22768717-6");
	Analytics::getInstance().setEnabled(settings->value("send_usage_data", true).toBool());
	Analytics::getInstance().sendEvent("lifecycle", "start");

	if (!gui) {
		QString blacklistOverride = parser.value(tagsBlacklistOption);
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
			blacklistOverride.isEmpty() ? profile->getBlacklist() : Blacklist(blacklistOverride.split(' ')),
			parser.isSet(noDuplicatesOption),
			parser.value(tagsMinOption).toInt(),
			parser.value(tagsFormatOption));

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

		downloader->setQuit(true);
		QObject::connect(downloader, &Downloader::quit, qApp, &QApplication::quit);
	}
	#if !defined(USE_CLI)
		else {
			// Check for updates
			const int cfuInterval = settings->value("check_for_updates", 24 * 60 * 60).toInt();
			QDateTime lastCfu = settings->value("last_check_for_updates", QDateTime()).toDateTime();
			if (cfuInterval >= 0 && (!lastCfu.isValid() || lastCfu.addSecs(cfuInterval) <= QDateTime::currentDateTime())) {
				settings->setValue("last_check_for_updates", QDateTime::currentDateTime());

				bool shouldQuit = false;
				auto *updateDialog = new UpdateDialog(&shouldQuit);
				auto *el = new QEventLoop();
				QObject::connect(updateDialog, &UpdateDialog::noUpdateAvailable, el, &QEventLoop::quit);
				QObject::connect(updateDialog, &UpdateDialog::rejected, el, &QEventLoop::quit);

				updateDialog->checkForUpdates();
				el->exec();
				el->deleteLater();
				updateDialog->deleteLater();

				if (shouldQuit) {
					return 0;
				}
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

			auto *mainWindow = new MainWindow(profile);
			mainWindow->init(parser.positionalArguments(), params);
			mainWindow->show();
		}
	#endif

	return app.exec();
}
