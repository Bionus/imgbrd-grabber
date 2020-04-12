#include "tabs/monitors-tab.h"
#include <QMenu>
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

		ui->tableMonitors->setItem(i, 0, new QTableWidgetItem(getIcon(":/images/status/pending.png"), ""));
		ui->tableMonitors->setItem(i, 1, new QTableWidgetItem(monitor.query().toString()));
		ui->tableMonitors->setItem(i, 2, new QTableWidgetItem(monitor.site()->url()));
		ui->tableMonitors->setItem(i, 3, new QTableWidgetItem(QString::number(monitor.interval())));
		ui->tableMonitors->setItem(i, 4, new QTableWidgetItem(monitor.download() ? "Download" : ""));
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
