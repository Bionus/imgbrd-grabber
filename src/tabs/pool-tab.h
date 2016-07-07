#ifndef POOL_TAB_H
#define POOL_TAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include "ui/textedit.h"
#include "models/page.h"
#include "mainwindow.h"
#include "search-tab.h"



namespace Ui
{
	class poolTab;
	class mainWindow;
}



class mainWindow;

class poolTab : public searchTab
{
	Q_OBJECT

	public:
		explicit poolTab(int id, QMap<QString,Site*> *sites, QList<Favorite> favorites, mainWindow *parent);
		~poolTab();
		Ui::poolTab *ui;
		QString tags();
		QString wiki();
		QString site();
		int imagesPerPage();
		int columns();
		QString postFilter();

	public slots:
		void updateCheckboxes();
		// Search
		void firstPage();
		void previousPage();
		void nextPage();
		void lastPage();
		// Zooms
		void setTags(QString);
		void setPool(int id, QString site);
		void webZoom(int);
		// Loading
		void load();
		void finishedLoading(Page*);
		void finishedLoadingTags(Page*);
		void finishedLoadingPreview(Image*);
		// Batch
		void getPage();
		void getAll();
		void getSel();
		// Tag list
		void linkHovered(QString);
		void linkClicked(QString);
		void contextMenu();
		void openInNewTab();
		void openInNewWindow();
		void viewitlater();
		void unviewitlater();
		// History
		void historyBack();
		void historyNext();
		// Others
		void optionsChanged();
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();
		void setSite(QString);
		void setImagesPerPage(int ipp);
		void setColumns(int columns);
		void setPostFilter(QString postfilter);
		void focusSearch();

	signals:
		void batchAddGroup(QStringList);
		void batchAddUnique(QMap<QString,QString>);

	private:
		int								m_id;
		mainWindow						*m_parent;
		TextEdit						*m_search, *m_postFiltering;
		QCalendarWidget					*m_calendar;
		QMap<QString,Page*>				m_pages;
		QList<Image*>					m_images;
		int								m_pagemax;
		QString							m_lastTags, m_wiki;
		bool							m_sized, m_from_history, m_stop;
		int								m_page, m_history_cursor;
		QList<QGridLayout*>				m_layouts;
		QList<QMap<QString,QString> >	m_history;
		QStringList						m_modifiers;
};

#endif // POOL_TAB_H
