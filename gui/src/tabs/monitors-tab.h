#ifndef MONITORS_TAB_H
#define MONITORS_TAB_H

#include "tabs/search-tab.h"


namespace Ui
{
	class MonitorsTab;
}


class MainWindow;
class MonitorManager;
class Profile;

class MonitorsTab : public SearchTab
{
	Q_OBJECT

	public:
		explicit MonitorsTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent);
		~MonitorsTab() override;
		Ui::MonitorsTab *ui;

		// SearchTab
		QString tags() const override;
		void setTags(const QString &tags, bool preload = true) override;
		void write(QJsonObject &json) const override;

	public slots:
		// SearchTab
		void focusSearch() override;
		void load() override;
		void updateTitle() override;

	protected:
		void changeEvent(QEvent *event) override;

	protected slots:
		void refresh();

	private:
		MonitorManager *m_monitorManager;
};

#endif // MONITORS_TAB_H
