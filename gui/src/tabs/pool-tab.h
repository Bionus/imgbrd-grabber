#ifndef POOL_TAB_H
#define POOL_TAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include <QJsonObject>
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
		explicit poolTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~poolTab();
		Ui::poolTab *ui;
		QString tags() const;
		QList<Site*> loadSites() const override;
		void write(QJsonObject &json) const;

	public slots:
		// Zooms
		void setTags(QString);
		void setPool(int id, QString site);
		// Loading
		void load();
		bool validateImage(QSharedPointer<Image> img);
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();
		void setSite(QString);
		void focusSearch();

	private:
		int				m_id;
		TextEdit		*m_search;
		QCalendarWidget	*m_calendar;
		bool			m_sized;
};

#endif // POOL_TAB_H
