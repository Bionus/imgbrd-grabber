#include <QCoreApplication>
#include <QMap>
#include <QScopedPointer>
#include <QSettings>
#include <QString>
#include <QStringList>
#include "cli.h"
#include "functions.h"
#include "logger.h"
#include "models/profile.h"


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	Logger::getInstance().initialize();

	Profile profile(savePath());
	profile.purgeTemp(24 * 60 * 60);

	QMap<QString, QString> params;
	QStringList positionalArgs;
	return parseAndRunCliArgs(&app, &profile, false, params, positionalArgs);
}
