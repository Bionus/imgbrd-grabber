#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QDialog>
#include <QDateTime>



namespace Ui
{
	class favoriteWindow;
}



class favoriteWindow : public QDialog
{
	Q_OBJECT

	public:
		favoriteWindow(QString, int, QDateTime, QWidget *parent);
		~favoriteWindow();

	public slots:
		void save();
		void on_openButton_clicked();
		void on_buttonRemove_clicked();

	signals:
		void favoritesChanged();

	private:
		Ui::favoriteWindow *ui;
		int m_note;
		QString m_tag;
		QDateTime m_lastviewed;

};

#endif // FAVORITEWINDOW_H
