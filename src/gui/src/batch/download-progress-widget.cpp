#include "batch/download-progress-widget.h"
#include <ui_download-progress-widget.h>
#include "downloader/batch-downloader.h"


DownloadProgressWidget::DownloadProgressWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::DownloadProgressWidget)
{
	ui->setupUi(this);
}

DownloadProgressWidget::~DownloadProgressWidget()
{
	delete ui;
}


void DownloadProgressWidget::setDownloader(BatchDownloader *downloader)
{
	m_downloader = downloader;
	connect(m_downloader, &BatchDownloader::stepChanged, this, &DownloadProgressWidget::onStepChanged);
	connect(m_downloader, &BatchDownloader::imageDownloadFinished, this, &DownloadProgressWidget::onResultChanged);

	onStepChanged();
	onResultChanged();
}

void DownloadProgressWidget::onStepChanged()
{
	QString msg;
	switch (m_downloader->currentStep())
	{
		case BatchDownloader::NotStarted: msg = tr("Not started"); break;
		case BatchDownloader::Login: msg = tr("Logging in"); break;
		case BatchDownloader::PageDownload: msg = tr("Downloading pages"); break;
		case BatchDownloader::ImageDownload: msg = tr("Downloading images"); break;
		case BatchDownloader::Finished: msg = tr("Finished"); break;
		case BatchDownloader::Aborted: msg = tr("Stopped"); break;
	}
	ui->labelStep->setText(msg);
}

void DownloadProgressWidget::onResultChanged()
{
	ui->labelSuccess->setText(QString::number(m_downloader->downloadedCount(BatchDownloader::Downloaded)));
	ui->labelIgnored->setText(QString::number(m_downloader->downloadedCount(BatchDownloader::Ignored)));
	ui->labelExists->setText(QString::number(m_downloader->downloadedCount(BatchDownloader::AlreadyExists)));
	ui->labelNotFound->setText(QString::number(m_downloader->downloadedCount(BatchDownloader::NotFound)));
	//ui->labelSkipped->setText(QString::number(m_downloader->downloadedCount(BatchDownloader::Skipped)));
	ui->labelErrors->setText(QString::number(m_downloader->downloadedCount(BatchDownloader::Errors)));

	ui->progressTotal->setMaximum(m_downloader->totalCount());
	ui->progressTotal->setValue(m_downloader->downloadedCount());
}
