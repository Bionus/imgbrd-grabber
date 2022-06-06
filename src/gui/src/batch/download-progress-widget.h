#ifndef DOWNLOAD_PROGRESS_WIDGET_H
#define DOWNLOAD_PROGRESS_WIDGET_H

#include <QWidget>
#include <QList>
#include "downloader/download-query-group.h"


namespace Ui
{
	class DownloadProgressWidget;
}


class DownloadProgressWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit DownloadProgressWidget(QWidget *parent = nullptr);
		~DownloadProgressWidget() override;

	public slots:
		void setDownload(const DownloadQueryGroup &download);

	private:
		Ui::DownloadProgressWidget *ui;
};

#endif // DOWNLOAD_PROGRESS_WIDGET_H
