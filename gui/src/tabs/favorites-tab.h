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
		explicit favoritesTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~favoritesTab();
		Ui::favoritesTab *ui;
		QList<bool> sources();
		QString tags() const;
		QList<Site*> loadSites() const override;
		void write(QJsonObject &json) const override;

	public slots:
		// Zooms
		void setTags(QString);
		// Loading
		void load();
		bool validateImage(QSharedPointer<Image> img);
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
		void closeEvent(QCloseEvent*);
		void addTabFavorite(QString);
		void focusSearch();
		void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage = nullptr) override;
		void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage = nullptr) override;

	private:
		QDateTime m_loadFavorite;
		QString m_currentTags;
		bool m_sized;
		int m_currentFav;
};

#endif // FAVORITES_TAB_H
