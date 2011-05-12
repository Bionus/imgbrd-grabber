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
	QStringList tags;
	QMap<QString,QString> params;
	QMap<QString,QString> assoc;
	assoc["b"] = "booru";
	assoc["l"] = "limit";
	assoc["s"] = "page";
	assoc["p"] = "path";
	assoc["f"] = "filename";
	assoc["u"] = "user";
	assoc["w"] = "password";
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			QStringList p = QString(argv[i]).split("=");
			QString key = p.at(0).right(p.at(0).size()-1);
			if (key[0] == '-') { key = key.right(key.size()-1); }
			p.removeFirst();
			params.insert(assoc.value(key, key), p.join("="));
		}
		else
		{ tags.append(argv[i]); }
	}

	QApplication app(argc, argv);

	mainWindow *fenetre = new mainWindow(argv[0], tags, params);
	fenetre->show();

	return app.exec();
}
