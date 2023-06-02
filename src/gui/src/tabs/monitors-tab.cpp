#include "tabs/monitors-tab.h"
#include <QMenu>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QtMath>
#include <QSettings>
#include <QShortcut>
#include <ui_monitors-tab.h>
#include <algorithm>
#include "downloader/download-query-group.h"
#include "logger.h"
#include "main-window.h"
#include "models/profile.h"
#include "models/site.h"
#include "monitor-table-model.h"
#include "monitor-window.h"
#include "monitoring/monitor.h"
#include "monitoring/monitor-manager.h"


MonitorsTab::MonitorsTab(Profile *profile, MonitorManager *monitorManager, MonitoringCenter *monitoringCenter, MainWindow *parent)
	: QWidget(parent), ui(new Ui::MonitorsTab), m_profile(profile), m_settings(profile->getSettings()), m_monitorManager(monitorManager), m_monitoringCenter(monitoringCenter), m_parent(parent)
{
	ui->setupUi(this);

	auto monitorTableModel = new MonitorTableModel(m_monitorManager, m_settings, this);
	m_monitorTableModel = new QSortFilterProxyModel(this);
	m_monitorTableModel->setSortRole(Qt::UserRole);
	m_monitorTableModel->setSourceModel(monitorTableModel);
	ui->tableMonitors->setModel(m_monitorTableModel);
	connect(m_monitoringCenter, &MonitoringCenter::statusChanged, monitorTableModel, &MonitorTableModel::setStatus);

	QShortcut *actionRemoveSelected = new QShortcut(QKeySequence::Delete, ui->tableMonitors);
	actionRemoveSelected->setContext(Qt::WidgetWithChildrenShortcut);
	connect(actionRemoveSelected, &QShortcut::activated, this, &MonitorsTab::removeSelected);

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
	menu->addAction(QIcon(":/images/icons/copy.png"), tr("Copy to downloads"), [this]() { convertSelected(); });
	menu->addAction(QIcon(":/images/icons/start.png"), tr("Start now"), [this]() { startSelected(); });
	menu->addSeparator();
	menu->addAction(QIcon(":/images/icons/remove.png"), tr("Remove"), [this]() { removeSelected(); });
	menu->exec(QCursor::pos());
}

void MonitorsTab::startSelected()
{
	QSet<int> rows;
	for (const QModelIndex &index : ui->tableMonitors->selectionModel()->selection().indexes()) {
		rows.insert(m_monitorTableModel->mapToSource(index).row());
	}

	for (const int row : rows) {
		m_monitorManager->monitors()[row].setForceRun();
	}

	m_monitoringCenter->tick();
}

void MonitorsTab::removeSelected()
{
	QList<int> rows;
	for (const QModelIndex &index : ui->tableMonitors->selectionModel()->selection().indexes()) {
		const int row = index.row();
		if (!rows.contains(row)) {
			rows.append(row);
		}
	}

	std::sort(rows.begin(), rows.end(), std::greater<int>());

	for (int i : qAsConst(rows)) {
		m_monitorTableModel->removeRow(i);
	}
}

void MonitorsTab::convertSelected()
{
	QSet<int> rows;
	for (const QModelIndex &index : ui->tableMonitors->selectionModel()->selection().indexes()) {
		rows.insert(m_monitorTableModel->mapToSource(index).row());
	}

	for (const int row : rows) {
		const Monitor &monitor = m_monitorManager->monitors()[row];
		for (Site *site : monitor.sites()) {
			emit batchAddGroup(DownloadQueryGroup(m_settings, monitor.query(), 1, 200, -1, monitor.postFilters(), site));
		}
	}
}

void MonitorsTab::toggleMonitoring()
{
	bool running = m_monitoringCenter->isRunning();
	if (running) {
		m_monitoringCenter->stop();
	} else {
		m_monitoringCenter->start();
	}
	ui->buttonToggle->setText(running ? tr("Start") : tr("Stop"));
}

void MonitorsTab::startNow()
{
	for (Favorite &fav : m_profile->getFavorites()) {
		for (Monitor &monitor : fav.getMonitors()) {
			monitor.setForceRun();
		}
	}
	for (auto &monitor : m_monitorManager->monitors()) {
		monitor.setForceRun();
	}

	m_monitoringCenter->tick();
}
