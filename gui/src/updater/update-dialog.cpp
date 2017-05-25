#include "update-dialog.h"
#include "ui_update-dialog.h"
#include <QMessageBox>
#include <QProcess>
#ifndef Q_OS_WIN
	#include <QDesktopServices>
#endif


UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(Q_NULLPTR), ui(new Ui::UpdateDialog), m_parent(parent)
{
	ui->setupUi(this);

	ui->labelChangelog->setVisible(ui->checkShowChangelog->isChecked());
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
	{
		emit noUpdateAvailable();
		return;
	}

	ui->labelChangelog->setText(changelog);
	ui->labelVersion->setText(tr("Version <b>%1</b>").arg(newVersion));

	show();
	activateWindow();
}

void UpdateDialog::accept()
{
	#ifdef Q_OS_WIN
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

	int reponse = QMessageBox::question(this, tr("Updater"), tr("To go on with the update, the program must be closed. Do you want to close now?"), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes)
	{
		QProcess::startDetached(path);

		if (m_parent != Q_NULLPTR)
		{ m_parent->close();}

		qApp->exit();
	}
	else
	{
		close();
	}
}
