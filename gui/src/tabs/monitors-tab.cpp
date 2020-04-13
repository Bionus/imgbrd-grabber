#include "tabs/monitors-tab.h"
#include <QMenu>
#include <QtMath>
#include <ui_monitors-tab.h>
#include <algorithm>
#include "logger.h"
#include "main-window.h"
#include "models/monitor.h"
#include "models/monitor-manager.h"
#include "models/profile.h"
#include "models/site.h"
#include "monitor-window.h"


MonitorsTab::MonitorsTab(Profile *profile, MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent)
	: QWidget(parent), ui(new Ui::MonitorsTab), m_profile(profile), m_monitorManager(monitorManager), m_monitoringCenter(monitoringCenter), m_parent(parent)
{
	ui->setupUi(this);

	connect(m_monitoringCenter, &MonitoringCenter::statusChanged, this, &MonitorsTab::statusChanged);
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


QIcon &MonitorsTab::getIcon(const QString &path)
{
	if (!m_icons.contains(path)) {
		m_icons.insert(path, QIcon(path));
	}

	return m_icons[path];
}

void MonitorsTab::statusChanged(const Monitor &monitor, MonitoringCenter::MonitoringStatus status)
{
	int row = m_monitorManager->monitors().indexOf(monitor);
	if (row == -1) {
		return;
	}

	QString path;
	switch (status)
	{
		case MonitoringCenter::MonitoringStatus::Waiting: path = ":/images/status/pending.png"; break;
		case MonitoringCenter::MonitoringStatus::Checking: path = ":/images/status/downloading.png"; break;
		case MonitoringCenter::MonitoringStatus::Performing: path = ":/images/status/ok.png"; break;
		default: return;
	}

	ui->tableMonitors->item(row, 0)->setIcon(getIcon(path));
}

void MonitorsTab::refresh()
{
	const QList<Monitor> &monitors = m_monitorManager->monitors();

	ui->tableMonitors->setRowCount(monitors.count());

	for (int i = 0; i < monitors.count(); ++i) {
		const Monitor &monitor = monitors[i];

		const int interval = qFloor(monitor.interval() / 60.0);
		const int days = interval / 1440;
		const int mins = interval % 1440;
		const QString timeFormat = mins > 60 ? (mins % 60 != 0 ? tr("h 'h' m 'm'") : tr("h 'h'")) : tr("m 'm'");

		QString sDate = days > 0 ? QString("%1 d ").arg(days) : "";
		QString sTime = mins > 0 ? QTime(0, 0, 0).addSecs(mins * 60).toString(timeFormat) : "";

		QStringList actions;
		if (monitor.notify()) {
			actions.append("Notify");
		}
		if (monitor.download()) {
			actions.append("Download");
		}

		QStringList sites;
		for (auto site : monitor.sites()) {
			sites.append(site->url());
		}

		ui->tableMonitors->setItem(i, 0, new QTableWidgetItem(getIcon(":/images/status/pending.png"), ""));
		ui->tableMonitors->setItem(i, 1, new QTableWidgetItem(monitor.query().toString()));
		ui->tableMonitors->setItem(i, 2, new QTableWidgetItem(sites.join(", ")));
		ui->tableMonitors->setItem(i, 3, new QTableWidgetItem(sDate + sTime));
		ui->tableMonitors->setItem(i, 4, new QTableWidgetItem(actions.join(", ")));
	}
}

void MonitorsTab::monitorsTableContextMenu(const QPoint &pos)
{
	auto *item = ui->tableMonitors->itemAt(pos);
	if (item == nullptr) {
		return;
	}

	int row = item->row();
	const Monitor &monitor = m_monitorManager->monitors()[row];

	auto *menu = new QMenu(this);
	menu->addAction(getIcon(":/images/icons/edit.png"), tr("Edit"), [this, monitor]() { (new MonitorWindow(m_profile, monitor, this))->show(); });
	menu->addAction(getIcon(":/images/icons/remove.png"), tr("Remove"), [this, monitor]() { m_monitorManager->remove(monitor); });
	menu->exec(QCursor::pos());
}
