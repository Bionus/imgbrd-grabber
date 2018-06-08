#ifndef FAVORITES_TAB_H
#define FAVORITES_TAB_H

#include <QMap>
#include <QWidget>
#include "tabs/search-tab.h"


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
		explicit favoritesTab(Profile *profile, mainWindow *parent);
		~favoritesTab() override;
		Ui::favoritesTab *ui;
		QList<Site*> sources() override;
		QString tags() const override;
		void write(QJsonObject &json) const override;

	protected:
		void changeEvent(QEvent *event) override;

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
		void setFavoriteViewed(const QString &tag);
		void viewed();
		// Others
		void closeEvent(QCloseEvent*) override;
		void focusSearch() override;
		void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, bool merged, const QString &noResultsMessage = nullptr) override;
		void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, const QString &noResultsMessage = nullptr) override;
		void updateTitle() override;

	private:
		QDateTime m_loadFavorite;
		QString m_currentTags;
		int m_currentFav;
		FixedSizeGridLayout *m_favoritesLayout;
};

#endif // FAVORITES_TAB_H
