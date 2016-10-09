#ifndef TAG_TAB_H
#define TAG_TAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include "ui/textedit.h"
#include "search-tab.h"
#include "models/page.h"
#include "mainwindow.h"
#include "downloader/downloader.h"



namespace Ui
{
	class tagTab;
}



class mainWindow;

class tagTab : public searchTab
{
	Q_OBJECT

	public:
		explicit tagTab(int id, QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent);
		~tagTab();
		Ui::tagTab *ui;
		QString tags();
		QString results();
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
		// Others
		void optionsChanged();
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();
		void setImagesPerPage(int ipp);
		void setColumns(int columns);
		void setPostFilter(QString postfilter);
		void focusSearch();

	private:
		int				m_id;
		TextEdit		*m_search, *m_postFiltering;
		QCalendarWidget	*m_calendar;
		QString			m_link, m_lastTags, m_wiki;
		bool			m_sized;
		Downloader		*m_downloader;
};

#endif // TAG_TAB_H
