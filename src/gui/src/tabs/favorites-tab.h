#ifndef FAVORITES_TAB_H
#define FAVORITES_TAB_H

#include "tabs/search-tab.h"
#include <QDateTime>


namespace Ui
{
	class FavoritesTab;
}


class MainWindow;
class Page;
class QMenu;

class FavoritesTab : public SearchTab
{
	Q_OBJECT

	public:
		explicit FavoritesTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent);
		~FavoritesTab() override;
		Ui::FavoritesTab *ui;
		QList<Site*> sources() override;
		QString tags() const override;
		void write(QJsonObject &json) const override;

	protected:
		void changeEvent(QEvent *event) override;
		void thumbnailContextMenu(QMenu *menu, const QSharedPointer<Image> &img) override;

	public slots:
		// Zooms
		void setTags(const QString &tags, bool preload = true) override;
		// Loading
		void load() override;
		bool validateImage(const QSharedPointer<Image> &img, QString &error) override;
		// Batch
		void getPage();
		void getAll();
		// Favorites
		void favoriteProperties(const QString &name);
		void updateFavorites();
		void loadFavorite(const QString &name);
		void checkFavorites();
		void loadNextFavorite();
		void favoritesBack();
 		void setFavoriteViewed(const QString &tag, const QDateTime &date = {});
		void viewed();
		// Others
		void closeEvent(QCloseEvent *event) override;
		void focusSearch() override;
		void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, bool merged, int filteredImages, const QString &noResultsMessage = nullptr) override;
		void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, int filteredImages, const QString &noResultsMessage = nullptr) override;
		void updateTitle() override;
		void splitterMoved(int pos, int index);

	private:
		QDateTime m_loadFavorite;
		QString m_currentTags;
		int m_currentFav;
		FixedSizeGridLayout *m_favoritesLayout;
};

#endif // FAVORITES_TAB_H
