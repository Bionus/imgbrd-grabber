#include "progress-bar-delegate.h"
#include <QApplication>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionProgressBar>
#include <QStyleOptionViewItem>
#include "download-group-table-model.h"
#include "downloader/download-query-group.h"


ProgressBarDelegate::ProgressBarDelegate(DownloadGroupTableModel *parent)
	: QItemDelegate(parent)
{}

void ProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	// Only act on the 11th column
	if (index.column() != 11) {
		QItemDelegate::paint(painter, option, index);
		return;
	}

	// Set up a QStyleOptionProgressBar to precisely mimic the environment of a progress bar
	QStyleOptionProgressBar progressBarOption;
	progressBarOption.state = QStyle::State_Enabled;
	progressBarOption.direction = QApplication::layoutDirection();
	progressBarOption.rect = option.rect;
	progressBarOption.fontMetrics = QApplication::fontMetrics();
	progressBarOption.minimum = 0;
	progressBarOption.textAlignment = Qt::AlignCenter;
	progressBarOption.textVisible = true;

	// Set the progress
	const DownloadQueryGroup &download = qobject_cast<DownloadGroupTableModel*>(parent())->dataForRow(index.row());
	progressBarOption.progress = download.progressVal;
	progressBarOption.maximum = download.total;

	// Draw the progress bar onto the view
	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
}
