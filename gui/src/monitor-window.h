#ifndef MONITOR_WINDOW_H
#define MONITOR_WINDOW_H

#include <QDialog>
#include "models/monitor.h"


namespace Ui
{
	class MonitorWindow;
}


class MonitorManager;
class Profile;
class QWidget;

class MonitorWindow : public QDialog
{
	Q_OBJECT

	public:
		MonitorWindow(Profile *profile, Monitor monitor, QWidget *parent);
		~MonitorWindow() override;

	public slots:
		void save();
		void remove();

	private:
		Ui::MonitorWindow *ui;
		Profile *m_profile;
		MonitorManager *m_monitorManager;
		Monitor m_monitor;
};

#endif // MONITOR_WINDOW_H
