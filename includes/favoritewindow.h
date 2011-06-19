#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QtGui>
#include "mainwindow.h"
#include "ui_favoritewindow.h"



namespace Ui
{
	class favoriteWindow;
}

class favoriteWindow : public QDialog
{
    Q_OBJECT

	public:
		favoriteWindow(QString, int, QDateTime, mainWindow *parent);
		~favoriteWindow();

	public slots:
		void save();
		void on_openButton_clicked();

	private:
		mainWindow *m_parent;
		Ui::favoriteWindow *ui;
		int m_note;
		QString m_tag;
		QDateTime m_lastviewed;

};

#endif // FAVORITEWINDOW_H
