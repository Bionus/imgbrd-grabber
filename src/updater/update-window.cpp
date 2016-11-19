#include "update-window.h"
#include "ui_update-window.h"


UpdateWindow::UpdateWindow(QWidget *parent)
	: QDialog(Q_NULLPTR), m_parent(parent), ui(new Ui::UpdateWindow)
{
	ui->setupUi(this);

	connect(&m_updater, &ProgramUpdater::finished, this, &UpdateWindow::checkForUpdatesDone);
}

UpdateWindow::~UpdateWindow()
{
	delete ui;
}


void UpdateWindow::checkForUpdates()
{
	m_updater.checkForUpdates();
}

void UpdateWindow::checkForUpdatesDone(QString newVersion, bool available, QString changelog)
{
	if (!available)
		return;

	ui->labelChangelog->setText(changelog);
	ui->labelVersion->setText(tr("Version <b>%1</b>").arg(newVersion));

	show();
	activateWindow();
}


void UpdateWindow::downloadUpdate()
{
	m_updaterProgress = new QProgressDialog(tr("Downloading update..."), tr("Abort update"), 0, 100, this);
	m_updaterProgress->setWindowModality(Qt::WindowModal);

	m_updaterReply = m_updater.downloadUpdate();
	connect(m_updaterReply, &QNetworkReply::downloadProgress, this, &UpdateWindow::downloadProgress);
	connect(m_updaterReply, &QNetworkReply::finished, this, &UpdateWindow::downloadFinished);
}

void UpdateWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{

}

void UpdateWindow::downloadFinished()
{
	m_parent->close();
}
