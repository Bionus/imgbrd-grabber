#include "tabs/monitors-tab.h"
#include <QMenu>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QtMath>
#include <QSettings>
#include <ui_monitors-tab.h>
#include <algorithm>
#include "logger.h"
#include "main-window.h"
#include "models/monitor.h"
#include "models/monitor-manager.h"
#include "models/profile.h"
#include "models/site.h"
#include "monitor-table-model.h"
#include "monitor-window.h"


MonitorsTab::MonitorsTab(Profile *profile, MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent)
	: QWidget(parent), ui(new Ui::MonitorsTab), m_profile(profile), m_settings(profile->getSettings()), m_monitorManager(monitorManager), m_parent(parent)
{
	ui->setupUi(this);

	auto monitorTableModel = new MonitorTableModel(m_monitorManager, this);
	m_monitorTableModel = new QSortFilterProxyModel(this);
	m_monitorTableModel->setSourceModel(monitorTableModel);
	ui->tableMonitors->setModel(m_monitorTableModel);
	connect(monitoringCenter, &MonitoringCenter::statusChanged, monitorTableModel, &MonitorTableModel::setStatus);

	ui->tableMonitors->loadGeometry(m_settings, "Monitoring", QList<int> { 0, 1, 2, 3, 4 });
}

MonitorsTab::~MonitorsTab()
{
	close();

	delete ui;
	delete m_monitorTableModel;
}

void MonitorsTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void MonitorsTab::closeEvent(QCloseEvent *event)
{
	ui->tableMonitors->saveGeometry(m_settings, "Monitoring");

	QWidget::closeEvent(event);
}

void MonitorsTab::monitorsTableContextMenu(const QPoint &pos)
{
	auto index = ui->tableMonitors->indexAt(pos);
	if (!index.isValid()) {
		return;
	}

	int row = m_monitorTableModel->mapToSource(index).row();
	const Monitor &monitor = m_monitorManager->monitors()[row];

	auto *menu = new QMenu(this);
	menu->addAction(QIcon(":/images/icons/edit.png"), tr("Edit"), [this, monitor]() { (new MonitorWindow(m_profile, monitor, this))->show(); });
	menu->addAction(QIcon(":/images/icons/remove.png"), tr("Remove"), [this, monitor]() { m_monitorManager->remove(monitor); });
	menu->exec(QCursor::pos());
}
