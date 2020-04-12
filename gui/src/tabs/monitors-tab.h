#ifndef MONITORS_TAB_H
#define MONITORS_TAB_H

#include <QWidget>


namespace Ui
{
	class MonitorsTab;
}


class MainWindow;
class MonitoringCenter;
class MonitorManager;
class Profile;

class MonitorsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit MonitorsTab(MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent);
		~MonitorsTab() override;
		Ui::MonitorsTab *ui;

	protected:
		void changeEvent(QEvent *event) override;

	protected slots:
		void refresh();

	private:
		MonitorManager *m_monitorManager;
		MonitoringCenter *m_monitoringCenter;
		MainWindow *m_parent;
};

#endif // MONITORS_TAB_H
