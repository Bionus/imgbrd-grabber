#ifndef FAVORITES_TAB_H
#define FAVORITES_TAB_H

#include <QWidget>
#include <QMap>
#include "search-tab.h"


namespace Ui
{
	class favoritesTab;
}


class mainWindow;
class Page;

class favoritesTab : public searchTab
{
	Q_OBJECT

	public:
		explicit favoritesTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~favoritesTab();
		Ui::favoritesTab *ui;
		QList<bool> sources() override;
		QString tags() const override;
		void write(QJsonObject &json) const override;

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Zooms
		void setTags(QString) override;
		// Loading
		void load() override;
		bool validateImage(QSharedPointer<Image> img) override;
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
		void closeEvent(QCloseEvent*) override;
		void focusSearch() override;
		void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage = nullptr) override;
		void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage = nullptr) override;

	private:
		QDateTime m_loadFavorite;
		QString m_currentTags;
		int m_currentFav;
};

#endif // FAVORITES_TAB_H
