#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QDialog>
#include <QDateTime>
#include "models/favorite.h"


namespace Ui
{
	class favoriteWindow;
}


class favoriteWindow : public QDialog
{
	Q_OBJECT

	public:
		favoriteWindow(Favorite, QWidget *parent);
		~favoriteWindow();

	public slots:
		void save();
		void on_openButton_clicked();
		void on_buttonRemove_clicked();

	signals:
		void favoritesChanged();

	private:
		Ui::favoriteWindow *ui;
		Favorite favorite;

};

#endif // FAVORITEWINDOW_H
