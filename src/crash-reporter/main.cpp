#include <QApplication>
#include "crash-reporter-window.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationDisplayName("Grabber");
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	CrashReporterWindow w;
	w.show();

	return app.exec();
}
