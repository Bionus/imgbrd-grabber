#ifndef SITEWINDOW_H
#define SITEWINDOW_H

#include <QDialog>
#include <QtNetwork>
#include "site.h"



namespace Ui
{
	class siteWindow;
}



class siteWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit siteWindow(QMap<QString,Site*> *sites, QWidget *parent = 0);
		~siteWindow();

	public slots:
		void accept();

	private:
		Ui::siteWindow *ui;
		QMap<QString,Site*> *m_sites;
};

#endif // SITEWINDOW_H
