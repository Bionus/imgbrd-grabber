#include <QApplication>
#include "mainwindow.h"



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationDisplayName("Grabber");
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	MainWindow w;
	w.show();
	
	return app.exec();
}
