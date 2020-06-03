#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "functions.h"
#include "main-screen.h"
#include "models/profile.h"


int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	const QUrl url(QStringLiteral("qrc:/main-screen.qml"));

	QQmlApplicationEngine engine;
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl) {
			QCoreApplication::exit(-1);
		}
	}, Qt::QueuedConnection);

	Profile profile(savePath());
	MainScreen mainScreen(&profile);
	engine.rootContext()->setContextProperty("backend", &mainScreen);

	engine.load(url);

	return app.exec();
}
