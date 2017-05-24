#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QDialog>
#include <QCalendarWidget>


namespace Ui
{
	class SearchWindow;
}


class Profile;
class TextEdit;

class SearchWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SearchWindow(QString tags, Profile *profile, QWidget *parent = 0);
		~SearchWindow();

	private:
		QString generateSearch(QString additional = "") const;

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
		Profile				*m_profile;
};

#endif // SEARCHWINDOW_H
