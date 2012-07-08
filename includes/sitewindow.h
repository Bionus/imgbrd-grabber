#ifndef SITEWINDOW_H
#define SITEWINDOW_H

#include <QDialog>
#include <QtNetwork>



namespace Ui
{
	class siteWindow;
}



class siteWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit siteWindow(QMap<QString,QMap<QString,QString> > *sites, QWidget *parent = 0);
		~siteWindow();

	public slots:
		void accept();

	private:
		Ui::siteWindow *ui;
		QMap<QString,QMap<QString,QString> > *m_sites;
};

#endif // SITEWINDOW_H
