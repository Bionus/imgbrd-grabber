#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QtGui>
#include "mainwindow.h"
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
		void openOptions();

	private:
		mainWindow *m_parent;
		Ui::startWindow *ui;

};

#endif // STARTWINDOW_H
