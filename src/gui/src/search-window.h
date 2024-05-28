#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include <QDialog>
#include <QString>


namespace Ui
{
	class SearchWindow;
}


class Profile;
class QCalendarWidget;
class TextEdit;

class SearchWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SearchWindow(QString tags, Profile *profile, QWidget *parent = nullptr);
		~SearchWindow() override;

	private:
		QString generateSearch(const QString &additional = "") const;

	public slots:
		void setDate(QDate d);
		void accept() override;
		void loadImage();

	signals:
		void accepted(const QString &search);

	private:
		Ui::SearchWindow *ui;
		QCalendarWidget *m_calendar;
		TextEdit *m_tags;
		Profile *m_profile;
};

#endif // SEARCH_WINDOW_H
