#ifndef POOL_TAB_H
#define POOL_TAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include <QJsonObject>
#include "search-tab.h"


namespace Ui
{
	class poolTab;
}


class mainWindow;
class TextEdit;

class poolTab : public searchTab
{
	Q_OBJECT

	public:
		explicit poolTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~poolTab();
		Ui::poolTab *ui;
		QString tags() const;
		QList<Site*> loadSites() const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json);

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Zooms
		void setTags(QString);
		void setPool(int id, QString site);
		// Loading
		void load();
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();
		void setSite(QString);
		void focusSearch();

	private:
		TextEdit *m_search;
};

#endif // POOL_TAB_H
