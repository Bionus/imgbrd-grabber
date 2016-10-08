#ifndef FAVORITES_TAB_H
#define FAVORITES_TAB_H

#include <QWidget>
#include <QMap>
#include "ui/textedit.h"
#include "search-tab.h"
#include "models/page.h"
#include "mainwindow.h"



namespace Ui
{
	class favoritesTab;
}



class mainWindow;

class favoritesTab : public searchTab
{
	Q_OBJECT

	public:
		explicit favoritesTab(int id, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~favoritesTab();
		Ui::favoritesTab *ui;
		QList<bool> sources();
		QString tags();
		QString wiki();
		int imagesPerPage();
		int columns();
		QString postFilter();

	public slots:
		// Search
		void firstPage();
		void previousPage();
		void nextPage();
		void lastPage();
		// Zooms
		void setTags(QString);
		// Loading
		void load();
		void finishedLoading(Page*);
		void failedLoading(Page*);
		void postLoading(Page*);
		void finishedLoadingTags(Page*);
		// Batch
		void getPage();
		void getAll();
		// Favorites
		void favoriteProperties(QString);
		void updateFavorites();
		void loadFavorite(QString);
		void checkFavorites();
		void loadNextFavorite();
		void favoritesBack();
		void setFavoriteViewed(QString);
		void viewed();
		// Others
		void optionsChanged();
		void closeEvent(QCloseEvent*);
		void toggleImage(int, bool);
		void addTabFavorite(QString);
		void setImagesPerPage(int);
		void setColumns(int);
		void setPostFilter(QString);
		void focusSearch();

	private:
		int				m_id;
		TextEdit		*m_postFiltering;
		QDateTime		m_loadFavorite;
		QList<Favorite>	&m_favorites;
		QString			m_lastTags, m_wiki, m_currentTags;
		bool			m_sized;
		int				m_page, m_currentFav;
};

#endif // FAVORITES_TAB_H
