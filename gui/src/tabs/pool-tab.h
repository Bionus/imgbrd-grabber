#ifndef POOL_TAB_H
#define POOL_TAB_H

#include <QCalendarWidget>
#include <QJsonObject>
#include <QMap>
#include <QWidget>
#include "tabs/search-tab.h"


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
		explicit poolTab(Profile *profile, mainWindow *parent);
		~poolTab() override;
		Ui::poolTab *ui;
		QString tags() const override;
		QList<Site*> loadSites() const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, bool preload = true);

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Zooms
		void setTags(const QString &tags, bool preload = true) override;
		void setPool(int id, const QString &site);
		// Loading
		void load() override;
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent*) override;
		void on_buttonSearch_clicked();
		void setSite(const QString &site);
		void focusSearch() override;
		void updateTitle() override;

	private:
		TextEdit *m_search;
};

#endif // POOL_TAB_H
