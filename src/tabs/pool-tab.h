#ifndef POOL_TAB_H
#define POOL_TAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include "ui/textedit.h"
#include "models/page.h"
#include "search-tab.h"



namespace Ui
{
	class poolTab;
}


class mainWindow;

class poolTab : public searchTab
{
	Q_OBJECT

	public:
		explicit poolTab(int id, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~poolTab();
		Ui::poolTab *ui;
		QString tags();
		QString wiki();
		QString site();
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
		void setPool(int id, QString site);
		// Loading
		void load();
		void finishedLoading(Page*);
		void finishedLoadingTags(Page*);
		void finishedLoadingPreview(Image*);
		// Batch
		void getPage();
		void getAll();
		// Tag list
		void linkHovered(QString);
		void linkClicked(QString);
		void contextMenu();
		void openInNewTab();
		void openInNewWindow();
		void viewitlater();
		void unviewitlater();
		// Others
		void optionsChanged();
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();
		void setSite(QString);
		void setImagesPerPage(int ipp);
		void setColumns(int columns);
		void setPostFilter(QString postfilter);
		void focusSearch();

	private:
		int					m_id;
		TextEdit			*m_search, *m_postFiltering;
		QCalendarWidget		*m_calendar;
		QMap<QString,Page*>	m_pages;
		int					m_pagemax;
		QString				m_lastTags, m_wiki;
		bool				m_sized, m_stop;
		int					m_page;
		QList<QGridLayout*>	m_layouts;
};

#endif // POOL_TAB_H
