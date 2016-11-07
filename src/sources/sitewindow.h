#ifndef SITEWINDOW_H
#define SITEWINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include "models/site.h"



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
		QList<Source*> *m_sources;
		QMap<QString ,Site*> *m_sites;
		QNetworkAccessManager *m_manager;
};

#endif // SITEWINDOW_H
