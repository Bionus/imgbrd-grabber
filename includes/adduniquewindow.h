#ifndef ADDUNIQUEWINDOW_H
#define ADDUNIQUEWINDOW_H

#include <QtGui>
#include <QtNetwork>
#include "mainwindow.h"



namespace Ui
{
	class AddUniqueWindow;
}



class AddUniqueWindow : public QDialog
{
	Q_OBJECT

	public:
		AddUniqueWindow(QString, QMap<QString,QMap<QString,QString> >, mainWindow *parent);

	public slots:
		void add();
		void ok(bool close = true);
		void replyFinished(Page *p);
		void addImage(Image *img);
		void on_buttonFolder_clicked();
		void on_lineFilename_textChanged(QString);

	private:
		Ui::AddUniqueWindow						*ui;
		mainWindow								*m_parent;
		Page									*m_page;
		QMap<QString,QMap<QString,QString> >	m_sites;
		bool									m_close;
};

#endif // ADDUNIQUEWINDOW_H
