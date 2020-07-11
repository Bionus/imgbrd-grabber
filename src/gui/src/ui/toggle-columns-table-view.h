#ifndef TOGGLE_COLUMNS_TABLE_VIEW_H
#define TOGGLE_COLUMNS_TABLE_VIEW_H

#include <QTableView>


class QPoint;
class QSettings;
class QWidget;

class ToggleColumnsTableView : public QTableView
{
	Q_OBJECT

	public:
		explicit ToggleColumnsTableView(QWidget *parent);

		void saveGeometry(QSettings *settings, const QString &group);
		void loadGeometry(QSettings *settings, const QString &group, const QList<int> &defaultColumns = {});

	public slots:
		void toggleColumn(int index);

	protected slots:
		void headersContextMenu(const QPoint &pos);
};

#endif // TOGGLE_COLUMNS_TABLE_VIEW_H
