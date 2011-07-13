#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QtGui>
#include <QtNetwork>
#include "ui_searchtab.h"
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
		// Others
		void optionsChanged(QSettings*);

	signals:
		void batchAddGroup(QStringList);
		void titleChanged(searchTab*);

	private:
		Ui::searchTab							*ui;
		TextEdit								*m_search, *m_postFiltering;
		QCalendarWidget							*m_calendar;
		QDateTime								*m_serverDate;
		QMap<QString,QString>					*m_favorites;
		QMap<QString,QMap<QString,QString> >	*m_sites;
		QList<Page*>							m_pages;
		QList<Image*>							m_images;
		QList<bool>								m_selectedSources;
		QList<QCheckBox*>						m_checkboxes;
		int										m_pagemax;
};

#endif // SEARCHTAB_H
