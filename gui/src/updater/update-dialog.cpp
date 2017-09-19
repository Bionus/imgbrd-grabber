#include "update-dialog.h"
#include "ui_update-dialog.h"
#include <QMessageBox>
#include <QProcess>
#include "functions.h"
#if !defined Q_OS_WIN || defined NIGHTLY
	#include <QDesktopServices>
#endif


UpdateDialog::UpdateDialog(bool *shouldQuit, QWidget *parent)
	: QDialog(Q_NULLPTR), ui(new Ui::UpdateDialog), m_shouldQuit(shouldQuit), m_parent(parent)
{
	ui->setupUi(this);

	ui->scrollArea->setVisible(ui->checkShowChangelog->isChecked());
	ui->progressDownload->hide();
	resize(300, 0);

	connect(&m_updater, &ProgramUpdater::finished, this, &UpdateDialog::checkForUpdatesDone);
}

UpdateDialog::~UpdateDialog()
{
	delete ui;
}

void UpdateDialog::resizeToFit()
{
	ui->scrollArea->setVisible(ui->checkShowChangelog->isChecked());
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
	{
		emit noUpdateAvailable();
		return;
	}

	bool hasChangelog = !changelog.isEmpty();
	if (hasChangelog)
	{
		ui->labelChangelog->setTextFormat(Qt::RichText);
		ui->labelChangelog->setText(parseMarkdown(changelog));
	}
	ui->checkShowChangelog->setVisible(hasChangelog);
	ui->scrollArea->setVisible(hasChangelog);
	ui->labelVersion->setText(tr("Version <b>%1</b>").arg(newVersion));

	show();
	activateWindow();
}

void UpdateDialog::accept()
{
	#if defined Q_OS_WIN && !defined NIGHTLY
		downloadUpdate();
	#else
		QDesktopServices::openUrl(m_updater.latestUrl());
		close();
	#endif
}


void UpdateDialog::downloadUpdate()
{
	ui->progressDownload->show();

	m_updater.downloadUpdate();
	connect(&m_updater, &ProgramUpdater::downloadProgress, this, &UpdateDialog::downloadProgress);
	connect(&m_updater, &ProgramUpdater::downloadFinished, this, &UpdateDialog::downloadFinished);
}

void UpdateDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	ui->progressDownload->setMaximum(bytesTotal);
	ui->progressDownload->setValue(bytesReceived);
}

void UpdateDialog::downloadFinished(QString path)
{
	ui->progressDownload->setValue(ui->progressDownload->maximum());

	QProcess::startDetached(path);

	if (m_parent != Q_NULLPTR)
	{ m_parent->close(); }

	*m_shouldQuit = true;
	qApp->exit();
}
