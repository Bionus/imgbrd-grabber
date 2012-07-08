#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include "textedit.h"
#include "page.h"
#include "mainwindow.h"



namespace Ui
{
	class searchTab;
	class mainWindow;
}



class mainWindow;

class searchTab : public QWidget
{
	Q_OBJECT

	public:
		explicit searchTab(int id, QMap<QString, QMap<QString, QString> > *sites, QMap<QString, QString> *favorites, QDateTime *serverDate, mainWindow *parent);
		~searchTab();
		Ui::searchTab *ui;
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
		// Tag list
		void linkHovered(QString);
		void linkClicked(QString);
		void contextMenu();
		void openInNewTab();
		void openInNewWindow();
		void favorite();
		void unfavorite();
		void viewitlater();
		void unviewitlater();
		// History
		void historyBack();
		void historyNext();
		// Others
		void optionsChanged();
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();

	signals:
		void batchAddGroup(QStringList);
		void titleChanged(searchTab*);
		void changed(searchTab*);
		void closed(searchTab*);

	private:
		int										m_id;
		mainWindow								*m_parent;
		TextEdit								*m_search, *m_postFiltering;
		QCalendarWidget							*m_calendar;
		QDateTime								*m_serverDate;
		QMap<QString,QString>					*m_favorites;
		QMap<QString,QMap<QString,QString> >	*m_sites;
		QMap<QString,Page*>						m_pages;
		QList<Image*>							m_images;
		int										m_pagemax;
		QList<bool>								m_selectedSources;
		QList<QCheckBox*>						m_checkboxes;
		QString									m_link, m_lastTags, m_wiki, m_tags;
		bool									m_sized, m_from_history, m_stop;
		int										m_page, m_history_cursor;
		QList<QGridLayout*>						m_layouts;
		QList<QMap<QString,QString> >			m_history;
		QStringList								m_modifiers;
};

#endif // SEARCHTAB_H
