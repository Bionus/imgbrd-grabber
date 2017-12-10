#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QDateTime>
#include <QDialog>
#include "models/favorite.h"


namespace Ui
{
	class favoriteWindow;
}


class Profile;

class favoriteWindow : public QDialog
{
	Q_OBJECT

	public:
		favoriteWindow(Profile *profile, const Favorite &favorite, QWidget *parent);
		~favoriteWindow() override;

	public slots:
		void save();
		void on_openButton_clicked();
		void on_buttonRemove_clicked();

	private:
		Ui::favoriteWindow	*ui;
		Profile				*m_profile;
		Favorite			m_favorite;
};

#endif // FAVORITEWINDOW_H
