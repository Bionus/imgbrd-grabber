#ifndef FAVORITE_WINDOW_H
#define FAVORITE_WINDOW_H

#include <QDialog>
#include <QWidget>
#include "models/favorite.h"


namespace Ui
{
	class FavoriteWindow;
}


class Profile;
class Site;

class FavoriteWindow : public QDialog
{
	Q_OBJECT

	public:
		FavoriteWindow(Profile *profile, Favorite favorite, QWidget *parent);
		~FavoriteWindow() override;

	public slots:
		void save();
		void on_openButton_clicked();
		void on_buttonRemove_clicked();
		void choosePathOverride();
		void openSourcesWindow();
		void setSources(const QList<Site*> &sources);

	private:
		Ui::FavoriteWindow *ui;
		Profile *m_profile;
		Favorite m_favorite;
		QList<Site*> m_selectedSources;
};

#endif // FAVORITE_WINDOW_H
