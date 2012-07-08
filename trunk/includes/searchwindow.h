#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QDialog>
#include <QCalendarWidget>
#include "textedit.h"



namespace Ui
{
	class SearchWindow;
}



class SearchWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SearchWindow(QString tags, QDate server, QWidget *parent = 0);
		~SearchWindow();

	public slots:
		void setDate(QDate d);
		void accept();
		void on_buttonImage_clicked();

	signals:
		void accepted(QString);

	private:
		Ui::SearchWindow	*ui;
		QCalendarWidget		*m_calendar;
		TextEdit			*m_tags;
};

#endif // SEARCHWINDOW_H
