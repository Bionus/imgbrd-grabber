#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include <QDialog>
#include <QNetworkReply>



namespace Ui
{
	class AboutWindow;
}



class AboutWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit AboutWindow(QString v, QWidget *parent = 0);
		~AboutWindow();

	public slots:
		void finished(QNetworkReply *r);

	private:
		Ui::AboutWindow *ui;
		int m_version;
};

#endif // ABOUT_WINDOW_H
