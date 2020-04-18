#ifndef PROGRESS_BAR_DELEGATE_H
#define PROGRESS_BAR_DELEGATE_H

#include <QItemDelegate>


class DownloadGroupTableModel;
class QModelIndex;
class QPainter;
class QStyleOptionViewItem;

class ProgressBarDelegate : public QItemDelegate
{
	Q_OBJECT

	public:
		explicit ProgressBarDelegate(DownloadGroupTableModel *parent = nullptr);
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // PROGRESS_BAR_DELEGATE_H
