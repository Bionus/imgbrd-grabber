#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QtGui>
#include <QtNetwork>
#include "textedit.h"
#include "page.h"



namespace Ui
{
    class searchTab;
}

class searchTab : public QWidget
{
    Q_OBJECT

	public:
		explicit searchTab(QMap<QString,QMap<QString,QString> > *sites, QMap<QString,QString> *favorites, QDateTime *serverDate, QWidget *parent = 0);
		~searchTab();
		Ui::searchTab *ui;

	public slots:
		// Search
		void firstPage();
		void previousPage();
		void nextPage();
		void lastPage();
		void insertDate(QDate);
		// Zooms
		void setTags(QString);
		void webZoom(int);
		// Loading
		void load();
		void finishedLoading(Page*);
		void finishedLoadingPreview(Image*);
		// Sources
		void openSourcesWindow();
		void saveSources(QList<bool>);
		void updateCheckboxes();
		// Batch
		void getPage();
		// Tag list
		void linkHovered(QString);
		void linkClicked(QString);
		void contextMenu();
		void openInNewWindow();
		void favorite();
		void unfavorite();
		void viewitlater();
		void unviewitlater();
		// Others
		void optionsChanged(QSettings*);
		void closeEvent(QCloseEvent*);

	signals:
		void batchAddGroup(QStringList);
		void titleChanged(searchTab*);

	private:
		TextEdit								*m_search, *m_postFiltering;
		QCalendarWidget							*m_calendar;
		QDateTime								*m_serverDate;
		QMap<QString,QString>					*m_favorites;
		QMap<QString,QMap<QString,QString> >	*m_sites;
		QList<Page*>							m_pages;
		QList<Image*>							m_images;
		int										m_pagemax;
		QList<bool>								m_selectedSources;
		QList<QCheckBox*>						m_checkboxes;
		QString									m_link;
		bool									m_sized;
};

#endif // SEARCHTAB_H
