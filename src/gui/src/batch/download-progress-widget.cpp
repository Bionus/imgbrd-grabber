#include "batch/download-progress-widget.h"
#include <ui_download-progress-widget.h>


DownloadProgressWidget::DownloadProgressWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::DownloadProgressWidget)
{
	ui->setupUi(this);
}

DownloadProgressWidget::~DownloadProgressWidget()
{
	delete ui;
}


void DownloadProgressWidget::setDownload(const DownloadQueryGroup &download)
{
	ui->labelSuccess->setText(download.query.toString());
}
