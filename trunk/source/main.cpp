/**
 * @file
 * @author  Bionus <bio.nus@hotmail.fr>
 * @version 3.1.0
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
#include <QtGui>
#include "QAffiche.h"
#include "QBouton.h"
#include "mainwindow.h"
#include "zoomwindow.h"
#include "optionswindow.h"



QMap<QDateTime,QString> _log;
mainWindow *_mainwindow;

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
	assoc["i"] = "ignore";
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

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	_mainwindow = new mainWindow(argv[0], tags, params);
	_mainwindow->init();
	_mainwindow->show();

	return app.exec();
}
