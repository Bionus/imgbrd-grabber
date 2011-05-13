#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QtGui>
#include "mainWindow.h"


namespace Ui
{
	class favoriteWindow;
}


class favoriteWindow : public QDialog
{
    Q_OBJECT

	public:
		explicit favoriteWindow(QString, int, QDateTime, QString, mainWindow *parent);
		~favoriteWindow();

	public slots:
		void save();
		void on_openButton_clicked();

	private:
		mainWindow *m_parent;
		Ui::favoriteWindow *ui;
		int m_note;
		QString m_tag, m_imagepath;
		QDateTime m_lastviewed;

};

#endif // FAVORITEWINDOW_H
