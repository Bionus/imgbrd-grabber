#include <QApplication>
#include <QtGui>
#include "QAffiche.h"
#include "QBouton.h"
#include "mainWindow.h"
#include "zoomWindow.h"
#include "optionsWindow.h"
#include "advancedWindow.h"



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QStringList params;
	for (int i = 1; i < argc; i++)
	{ params.append(argv[i]); }

	mainWindow *fenetre = new mainWindow(argv[0], params);
    fenetre->show();

	return app.exec();
}
