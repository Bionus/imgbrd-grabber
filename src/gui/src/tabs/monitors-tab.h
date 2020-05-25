#ifndef MONITORS_TAB_H
#define MONITORS_TAB_H

#include <QWidget>
#include "monitoring-center.h"


namespace Ui
{
	class MonitorsTab;
}


class MainWindow;
class Monitor;
class MonitorManager;
class Profile;
class QCloseEvent;
class QEvent;
class QPoint;
class QSettings;
class QSortFilterProxyModel;

class MonitorsTab : public QWidget
{
	Q_OBJECT

	public:
		MonitorsTab(Profile *profile, MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent);
		~MonitorsTab() override;
		Ui::MonitorsTab *ui;

	protected:
		void changeEvent(QEvent *event) override;
		void closeEvent(QCloseEvent *event) override;

	protected slots:
		void monitorsTableContextMenu(const QPoint &pos);

	private:
		Profile *m_profile;
		QSettings *m_settings;
		MonitorManager *m_monitorManager;
		MainWindow *m_parent;
		QSortFilterProxyModel *m_monitorTableModel;
};

#endif // MONITORS_TAB_H
