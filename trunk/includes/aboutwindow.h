#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QtGui>
#include <QtNetwork>



namespace Ui
{
	class aboutWindow;
}

class aboutWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit aboutWindow(QString v, QWidget *parent = 0);
		~aboutWindow();

	public slots:
		void finished(QNetworkReply *r);

	private:
		Ui::aboutWindow *ui;
		int m_version;
};

#endif // ABOUTWINDOW_H
