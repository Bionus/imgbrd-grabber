#ifndef FAVORITESTAB_H
#define FAVORITESTAB_H

#include <QWidget>
#include <QMap>
#include "textedit.h"
#include "searchtab.h"
#include "page.h"
#include "mainwindow.h"



namespace Ui
{
	class favoritesTab;
    class mainWindow;
}



class mainWindow;

class favoritesTab : public searchTab
{
    Q_OBJECT

    public:
		explicit favoritesTab(int id, QMap<QString,Site*> *sites, QMap<QString, QString> *favorites, QDateTime *serverDate, mainWindow *parent);
		~favoritesTab();
		Ui::favoritesTab *ui;
        QList<bool> sources();
        QString tags();
        QString results();
        QString wiki();

    public slots:
        // Search
        void firstPage();
        void previousPage();
        void nextPage();
        void lastPage();
        // Zooms
        void setTags(QString);
        void webZoom(int);
        // Loading
        void load();
        void finishedLoading(Page*);
        void finishedLoadingTags(Page*);
        void finishedLoadingPreview(Image*);
        // Sources
        void openSourcesWindow();
        void saveSources(QList<bool>);
        void updateCheckboxes();
        // Batch
        void getPage();
        void getAll();
		void getSel();
		// Tag list
        void linkHovered(QString);
		void linkClicked(QString);
        void openInNewTab();
		void openInNewWindow();
        // History
        void historyBack();
        void historyNext();
		// Favorites
		void favoriteProperties(int id = -1);
		void updateFavorites();
		void loadFavorite(int);
		void checkFavorites();
		void loadNextFavorite();
		void favoritesBack();
		void setFavoriteViewed(QString);
		void viewed();
        // Others
        void optionsChanged();
        void closeEvent(QCloseEvent*);
		void toggleImage(int, bool);
		void addTabFavorite(int);

    signals:
		void batchAddGroup(QStringList);
		void batchAddUnique(QMap<QString,QString>);
		void changed(searchTab*);
		void closed(searchTab*);

    private:
		int								m_id;
		mainWindow						*m_parent;
		TextEdit						*m_postFiltering;
		QDateTime						*m_serverDate, m_loadFavorite;
		QMap<QString,QString>			*m_favorites;
		QMap<QString,Site*>				*m_sites;
		QMap<QString,Page*>				m_pages;
		QList<Image*>					m_images;
		int								m_pagemax;
		QList<bool>						m_selectedSources;
		QList<QCheckBox*>				m_checkboxes;
		QString							m_link, m_lastTags, m_wiki, m_tags, m_currentTags;
		bool							m_sized, m_from_history, m_stop;
		int								m_page, m_history_cursor, m_currentFav;
		QList<QGridLayout*>				m_layouts;
		QList<QMap<QString,QString> >	m_history;
		QStringList						m_modifiers;
};

#endif // FAVORITESTAB_H
