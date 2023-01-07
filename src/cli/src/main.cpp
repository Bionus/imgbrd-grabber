#include <QCoreApplication>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QStringList>
#include "functions.h"
#include "models/profile.h"
#include "cli.h"


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	auto *profile = new Profile(savePath());
	profile->purgeTemp(24 * 60 * 60);

	QMap<QString, QString> params;
	QStringList positionalArgs;
	return parseAndRunCliArgs(&app, profile, false, params, positionalArgs);
}
