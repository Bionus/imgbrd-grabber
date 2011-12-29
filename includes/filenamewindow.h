#ifndef FILENAMEWINDOW_H
#define FILENAMEWINDOW_H

#include <QtGui>
#include "functions.h"



namespace Ui
{
	class filenameWindow;
}

class filenameWindow : public QDialog
{
    Q_OBJECT

	public:
		explicit filenameWindow(QWidget *parent = 0);
		~filenameWindow();

	public slots:
		void accept();

	signals:
		void validated(QString, QString);

	private:
		Ui::filenameWindow *ui;
};

#endif // FILENAMEWINDOW_H
