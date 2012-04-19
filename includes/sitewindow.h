#ifndef SITEWINDOW_H
#define SITEWINDOW_H

#include <QtGui>
#include <QtNetwork>
#include "functions.h"



namespace Ui
{
	class siteWindow;
}

class siteWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit siteWindow(QStringMapMap *sites, QWidget *parent = 0);
		~siteWindow();

	public slots:
		void accept();

	private:
		Ui::siteWindow *ui;
		QStringMapMap *m_sites;
};

#endif // SITEWINDOW_H
