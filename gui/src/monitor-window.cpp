#include "monitor-window.h"
#include <QtMath>
#include <ui_monitor-window.h>
#include "models/monitor-manager.h"
#include "models/profile.h"
#include "models/site.h"


MonitorWindow::MonitorWindow(Profile *profile, Monitor monitor, QWidget *parent)
	: QDialog(parent), ui(new Ui::MonitorWindow), m_profile(profile), m_monitorManager(profile->monitorManager()), m_monitor(std::move(monitor))
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	QStringList sourceKeys = profile->getSites().keys();
	ui->comboSource->addItems(sourceKeys);
	ui->comboSource->setCurrentIndex(sourceKeys.indexOf(m_monitor.site()->url()));

	ui->lineSearch->setText(m_monitor.query().toString());
	ui->dateLastCheck->setDateTime(m_monitor.lastCheck());
	ui->spinInterval->setValue(qFloor(m_monitor.interval() / 60.0));

	ui->checkNotificationEnabled->setChecked(false); // FIXME

	ui->checkDownloadEnabled->setChecked(m_monitor.download());
	ui->lineDownloadPathOverride->setText(m_monitor.pathOverride());
	ui->lineDownloadFilenameOverride->setText(m_monitor.filenameOverride());

	connect(this, &QDialog::accepted, this, &MonitorWindow::save);
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
	m_monitorManager->remove(m_monitor);

	Site *site = m_profile->getSites().value(ui->comboSource->currentText());
	int interval = ui->spinInterval->value() * 60;
	bool download = ui->checkDownloadEnabled->isChecked();
	QString pathOverride = ui->lineDownloadPathOverride->text();
	QString filenameOverride = ui->lineDownloadFilenameOverride->text();

	Monitor newMonitor(site, interval, m_monitor.lastCheck(), download, pathOverride, filenameOverride, m_monitor.cumulated(), m_monitor.preciseCumulated(), m_monitor.query());
	m_monitorManager->add(newMonitor);
}
