#include "tabs/monitors-tab.h"
#include <ui_monitors-tab.h>
#include <algorithm>
#include "logger.h"
#include "main-window.h"
#include "models/monitor.h"
#include "models/monitor-manager.h"
#include "models/profile.h"
#include "models/site.h"


MonitorsTab::MonitorsTab(MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent)
	: QWidget(parent), ui(new Ui::MonitorsTab), m_monitorManager(monitorManager), m_monitoringCenter(monitoringCenter), m_parent(parent)
{
	ui->setupUi(this);

	connect(m_monitorManager, &MonitorManager::changed, this, &MonitorsTab::refresh);
	refresh();
}

MonitorsTab::~MonitorsTab()
{
	close();
	delete ui;
}

void MonitorsTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}


void MonitorsTab::refresh()
{
	const QList<Monitor> &monitors = m_monitorManager->monitors();

	ui->tableMonitors->setRowCount(monitors.count());

	for (int i = 0; i < monitors.count(); ++i) {
		const Monitor &monitor = monitors[i];

		ui->tableMonitors->setItem(i, 0, new QTableWidgetItem(""));
		ui->tableMonitors->setItem(i, 1, new QTableWidgetItem(monitor.site()->url()));
		ui->tableMonitors->setItem(i, 2, new QTableWidgetItem(QString::number(monitor.interval())));
		ui->tableMonitors->setItem(i, 3, new QTableWidgetItem(""));
	}
}
