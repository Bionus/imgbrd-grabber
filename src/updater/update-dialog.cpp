#include "update-dialog.h"
#include "ui_update-dialog.h"


UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(Q_NULLPTR), ui(new Ui::UpdateDialog), m_parent(parent)
{
	ui->setupUi(this);

	ui->labelChangelog->setVisible(ui->checkShowChangelog->isChecked());
	resize(300, 0);

	connect(&m_updater, &ProgramUpdater::finished, this, &UpdateDialog::checkForUpdatesDone);
}

UpdateDialog::~UpdateDialog()
{
	delete ui;
}

void UpdateDialog::resizeToFit()
{
	ui->labelChangelog->setVisible(ui->checkShowChangelog->isChecked());
	int width = ui->labelUpdateAvailable->size().width();

	ui->labelUpdateAvailable->setMinimumWidth(width);
	adjustSize();
	ui->labelUpdateAvailable->setMinimumWidth(0);
}


void UpdateDialog::checkForUpdates()
{
	m_updater.checkForUpdates();
}

void UpdateDialog::checkForUpdatesDone(QString newVersion, bool available, QString changelog)
{
	if (!available)
		return;

	ui->labelChangelog->setText(changelog);
	ui->labelVersion->setText(tr("Version <b>%1</b>").arg(newVersion));

	show();
	activateWindow();
}


void UpdateDialog::downloadUpdate()
{
	m_updaterProgress = new QProgressDialog(tr("Downloading update..."), tr("Abort update"), 0, 100, this);
	m_updaterProgress->setWindowModality(Qt::WindowModal);

	m_updaterReply = m_updater.downloadUpdate();
	connect(m_updaterReply, &QNetworkReply::downloadProgress, this, &UpdateDialog::downloadProgress);
	connect(m_updaterReply, &QNetworkReply::finished, this, &UpdateDialog::downloadFinished);
}

void UpdateDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	Q_UNUSED(bytesReceived)
	Q_UNUSED(bytesTotal)
}

void UpdateDialog::downloadFinished()
{
	m_parent->close();
}
