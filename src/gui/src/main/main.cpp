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
#include <QEventLoop>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslSocket>
#include <QString>
#include <QStringList>
#include "analytics.h"
#include "cli.h"
#include "functions.h"
#include "main-window.h"
#include "models/page-api.h"
#include "models/profile.h"
#include "updater/update-dialog.h"
#if !defined(USE_CLI) && defined(USE_BREAKPAD)
	#include <QFileInfo>
	#include "crashhandler/crashhandler.h"
#endif
#if defined(Q_OS_ANDROID)
	#include <QMessageBox>
	#include "android.h"
#endif
#include "custom-buttons.h" // Needed for ButtonSettings registration to work.

#ifdef WIN_FILE_PROPS
	#include "windows-file-property.h"
#endif


int main(int argc, char *argv[])
{
	#if defined(Q_OS_WIN)
		qputenv("QT_MEDIA_BACKEND", "windows");
	#endif

	#ifdef WIN_FILE_PROPS
		initializeWindowsProperties();
	#endif

	QApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	// Handler for custom URL protocols, redirecting to the main program through HTTP calls
	if (argc == 3 && QString(argv[1]) == "--url-protocol") {
		QNetworkAccessManager manager;
		QNetworkRequest request(QUrl("http://127.0.0.1:58923/" + QString(argv[2])));

		QEventLoop loop;
		QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
		manager.get(request);
		loop.exec();

		return 0;
	}

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
	const QStringList toCopy { "themes/", "webservices/" };
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

	// Ensure SSL libraries are loaded
	QSslSocket::supportsSsl();

	auto *profile = new Profile(savePath());
	profile->purgeTemp(24 * 60 * 60);
	QSettings *settings = profile->getSettings();

	// Default to the GUI unless USE_CLI is defined
	bool defaultToGui = true;
	#if defined(USE_CLI)
		defaultToGui = false;
	#endif

	// Parse CLI parameters
	QMap<QString, QString> params;
	QStringList positionalArgs;
	const int ret = parseAndRunCliArgs(&app, profile, defaultToGui, params, positionalArgs);
	if (ret != -1) {
		return ret;
	}

	// Google Breakpad to handle crashes
	#if defined(USE_BREAKPAD) && !defined(USE_CLI)
		QDir dir = QFileInfo(argv[0]).dir();
		QString crashes = savePath("crashes");
		if (!dir.exists(crashes)) {
			dir.mkpath(crashes);
		}
		CrashHandler::instance()->Init(crashes);
	#endif

	// Analytics
	Analytics::getInstance().setTrackingID("UA-22768717-6");
	Analytics::getInstance().setMeasurementID("G-15KZMHH7KG");
	Analytics::getInstance().setEnabled(settings->value("send_usage_data", true).toBool());
	Analytics::getInstance().startSession();
	Analytics::getInstance().sendEvent("lifecycle", "start");

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

	// Run the main window
	auto *mainWindow = new MainWindow(profile);
	mainWindow->init(positionalArgs, params);
	mainWindow->show();

	return app.exec();
}
