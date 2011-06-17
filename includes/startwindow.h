#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QtGui>
#include "mainWindow.h"
#include "ui_startwindow.h"



namespace Ui
{
	class startWindow;
}

class startWindow : public QDialog
{
	Q_OBJECT

	public:
		startWindow(mainWindow *parent);
		~startWindow();

	public slots:
		void save();
		void on_openButton_clicked();
		void on_filenameLineEdit_textChanged(QString);

	private:
		mainWindow *m_parent;
		Ui::startWindow *ui;

};

#endif // STARTWINDOW_H
