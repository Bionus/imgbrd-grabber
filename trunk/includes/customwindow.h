#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include <QtGui>
#include "functions.h"



namespace Ui
{
	class customWindow;
}

class customWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit customWindow(QWidget *parent = 0);
		~customWindow();

	public slots:
		void accept();

	signals:
		void validated(QString, QString);

	private:
		Ui::customWindow *ui;
};

#endif // CUSTOMWINDOW_H
