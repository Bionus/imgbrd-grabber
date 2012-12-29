#ifndef POOLTAB_H
#define POOLTAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include "textedit.h"
#include "page.h"
#include "mainwindow.h"



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
		explicit poolTab(int id, QMap<QString,Site*> *sites, QMap<QString, QString> *favorites, QDateTime *serverDate, mainWindow *parent);
        ~poolTab();
		Ui::poolTab *ui;
        QString tags();
        QString results();
        QString wiki();
		QString site();

    public slots:
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
		void setSite(QString);

    signals:
        void batchAddGroup(QStringList);
		void batchAddUnique(QMap<QString,QString>);
		void titleChanged(searchTab*);
		void changed(searchTab*);
		void closed(poolTab*);

    private:
		int								m_id;
		mainWindow						*m_parent;
		TextEdit						*m_search, *m_postFiltering;
		QCalendarWidget					*m_calendar;
		QDateTime						*m_serverDate;
		QMap<QString,QString>			*m_favorites;
		QMap<QString,Site*>				*m_sites;
		QMap<QString,Page*>				m_pages;
		QList<Image*>					m_images;
		int								m_pagemax;
		QList<QCheckBox*>				m_checkboxes;
		QString							m_link, m_lastTags, m_wiki, m_tags;
		bool							m_sized, m_from_history, m_stop;
		int								m_page, m_history_cursor;
		QList<QGridLayout*>				m_layouts;
		QList<QMap<QString,QString> >	m_history;
		QStringList						m_modifiers;
};

#endif // POOLTAB_H
