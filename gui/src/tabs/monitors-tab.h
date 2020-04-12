#ifndef MONITORS_TAB_H
#define MONITORS_TAB_H

#include <QMap>
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
class QIcon;
class QString;

class MonitorsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit MonitorsTab(Profile *profile, MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent);
		~MonitorsTab() override;
		Ui::MonitorsTab *ui;

	protected:
		void changeEvent(QEvent *event) override;
		QIcon &getIcon(const QString &path);

	protected slots:
		void refresh();
		void statusChanged(const Monitor &monitor, MonitoringCenter::MonitoringStatus status);
		void monitorsTableContextMenu(const QPoint &pos);

	private:
		Profile *m_profile;
		MonitorManager *m_monitorManager;
		MonitoringCenter *m_monitoringCenter;
		MainWindow *m_parent;
		QMap<QString, QIcon> m_icons;
};

#endif // MONITORS_TAB_H
