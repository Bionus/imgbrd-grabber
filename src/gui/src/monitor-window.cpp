#include "monitor-window.h"
#include <QtMath>
#include <ui_monitor-window.h>
#include "models/monitor-manager.h"
#include "models/profile.h"
#include "models/site.h"
#include "sources/sources-window.h"


MonitorWindow::MonitorWindow(Profile *profile, Monitor monitor, QWidget *parent)
	: QDialog(parent), ui(new Ui::MonitorWindow), m_profile(profile), m_monitorManager(profile->monitorManager()), m_monitor(std::move(monitor))
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_selectedSources = m_monitor.sites();

	ui->lineSearch->setText(m_monitor.query().toString());
	if (!m_monitor.query().gallery.isNull()) {
		ui->lineSearch->setEnabled(false);
	}

	ui->linePostFilters->setText(m_monitor.postFilters().join(' '));
	ui->dateLastCheck->setDateTime(m_monitor.lastCheck());
	ui->spinInterval->setValue(qFloor(m_monitor.interval() / 60.0));
	ui->spinDelay->setValue(qFloor(m_monitor.delay() / 60.0));

	ui->checkNotificationEnabled->setChecked(m_monitor.notify());

	ui->checkDownloadEnabled->setChecked(m_monitor.download());
	ui->lineDownloadPathOverride->setText(m_monitor.pathOverride());
	ui->lineDownloadFilenameOverride->setText(m_monitor.filenameOverride());
	ui->checkGetBlacklisted->setChecked(m_monitor.getBlacklisted());

	connect(this, &QDialog::accepted, this, &MonitorWindow::save);

	// You can't use notifications without the system tray icon
	const bool enableTray = m_profile->getSettings()->value("Monitoring/enableTray", false).toBool();
	if (!enableTray) {
		ui->checkNotificationEnabled->setEnabled(false);
		ui->checkNotificationEnabled->setToolTip(tr("You need to enable the system tray icon to use notifications."));
	}
}

MonitorWindow::~MonitorWindow()
{
	delete ui;
}

void MonitorWindow::remove()
{
	m_monitorManager->remove(m_monitor);
	close();
}

void MonitorWindow::save()
{
	int index = m_monitorManager->remove(m_monitor);

	SearchQuery query = !m_monitor.query().gallery.isNull() ? m_monitor.query() : ui->lineSearch->text().split(' ', Qt::SkipEmptyParts);
	QStringList postFilters = ui->linePostFilters->text().split(' ', Qt::SkipEmptyParts);
	int interval = ui->spinInterval->value() * 60;
	int delay = ui->spinDelay->value() * 60;
	bool notify = ui->checkNotificationEnabled->isChecked();
	bool download = ui->checkDownloadEnabled->isChecked();
	QString pathOverride = ui->lineDownloadPathOverride->text();
	QString filenameOverride = ui->lineDownloadFilenameOverride->text();
	bool getBlacklisted = ui->checkGetBlacklisted->isChecked();

	Monitor newMonitor(m_selectedSources, interval, m_monitor.lastCheck(), download, pathOverride, filenameOverride, m_monitor.cumulated(), m_monitor.preciseCumulated(), query, postFilters, notify, delay, getBlacklisted);
	m_monitorManager->add(newMonitor, index);
}

void MonitorWindow::openSourcesWindow()
{
	auto w = new SourcesWindow(m_profile, m_selectedSources, this);
	connect(w, SIGNAL(valid(QList<Site*>)), this, SLOT(setSources(QList<Site*>)));
	w->show();
}

void MonitorWindow::setSources(const QList<Site*> &sources)
{
	m_selectedSources = sources;
}
