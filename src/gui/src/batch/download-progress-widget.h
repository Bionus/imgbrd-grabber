#ifndef DOWNLOAD_PROGRESS_WIDGET_H
#define DOWNLOAD_PROGRESS_WIDGET_H

#include <QWidget>
#include <QList>
#include "downloader/download-query-group.h"


namespace Ui
{
	class DownloadProgressWidget;
}


class BatchDownloader;

class DownloadProgressWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit DownloadProgressWidget(QWidget *parent = nullptr);
		~DownloadProgressWidget() override;

	public slots:
		void setDownloader(BatchDownloader *downloader);

	protected slots:
		void onStepChanged();
		void onResultChanged();

	private:
		Ui::DownloadProgressWidget *ui;
		BatchDownloader *m_downloader;
};

#endif // DOWNLOAD_PROGRESS_WIDGET_H
