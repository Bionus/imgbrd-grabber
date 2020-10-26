#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include "functions.h"
#include "language-loader.h"
#include "main-screen.h"
#include "models/image.h"
#include "models/profile.h"
#include "settings.h"
#include "share/share-utils.h"
#include "syntax-highlighter-helper.h"


int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	qmlRegisterType<ShareUtils>("Grabber", 1, 0, "ShareUtils");
	qmlRegisterType<SyntaxHighlighterHelper>("Grabber", 1, 0, "SyntaxHighlighterHelper");
	qRegisterMetaType<QSharedPointer<Image>>("QSharedPointer<Image>");
	qRegisterMetaType<Settings*>("Settings*");

	// Copy settings files to writable directory
	const QStringList toCopy { "sites/", "themes/", "webservices/" };
	for (const QString &tgt : toCopy) {
		const QString from = savePath(tgt, true, false);
		const QString to = savePath(tgt, true, true);
		if (!QDir(to).exists() && QDir(from).exists()) {
			copyRecursively(from, to);
		}
	}

	const QUrl url(QStringLiteral("qrc:/main-screen.qml"));

	QQmlApplicationEngine engine;
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl) {
			QCoreApplication::exit(-1);
		}
	}, Qt::QueuedConnection);

	// Define a few globals
	engine.rootContext()->setContextProperty("VERSION", QString(VERSION));
	#ifdef NIGHTLY
		engine.rootContext()->setContextProperty("NIGHTLY", true);
		engine.rootContext()->setContextProperty("NIGHTLY_COMMIT", QString(NIGHTLY_COMMIT));
	#else
		engine.rootContext()->setContextProperty("NIGHTLY", false);
		engine.rootContext()->setContextProperty("NIGHTLY_COMMIT", QString());
	#endif

	Profile profile(savePath());
	MainScreen mainScreen(&profile, &engine);
	engine.setObjectOwnership(&mainScreen, QQmlEngine::CppOwnership);
	engine.rootContext()->setContextProperty("backend", &mainScreen);

	Settings settings(profile.getSettings());
	engine.rootContext()->setContextProperty("settings", &settings);

	// Load translations
	LanguageLoader languageLoader(savePath("languages/", true, false));
	languageLoader.install(qApp);
	languageLoader.setLanguage(profile.getSettings()->value("language", "English").toString());
	engine.rootContext()->setContextProperty("languageLoader", &languageLoader);
	#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
		QObject::connect(&languageLoader, &LanguageLoader::languageChanged, &engine, &QQmlEngine::retranslate);
	#endif

	engine.load(url);

	return app.exec();
}
