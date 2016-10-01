#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QDialog>
#include <QDateTime>
#include "models/favorite.h"
#include "models/profile.h"


namespace Ui
{
	class favoriteWindow;
}


class favoriteWindow : public QDialog
{
	Q_OBJECT

	public:
		favoriteWindow(Profile *profile, Favorite favorite, QWidget *parent);
		~favoriteWindow();

	public slots:
		void save();
		void on_openButton_clicked();
		void on_buttonRemove_clicked();

	signals:
		void favoritesChanged();

	private:
		Ui::favoriteWindow	*ui;
		Profile				*m_profile;
		Favorite			m_favorite;

};

#endif // FAVORITEWINDOW_H
