#include "ui/toggle-columns-table-view.h"
#include <QHeaderView>
#include <QMenu>
#include <QSettings>


ToggleColumnsTableView::ToggleColumnsTableView(QWidget *parent)
	: QTableView(parent)
{
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &ToggleColumnsTableView::headersContextMenu);
}


void ToggleColumnsTableView::saveGeometry(QSettings *settings, const QString &group)
{
	QStringList sizes;
	QStringList enabledColumns;

	sizes.reserve(model()->columnCount());
	for (int i = 0; i < model()->columnCount(); ++i) {
		sizes.append(QString::number(horizontalHeader()->sectionSize(i)));
		if  (!horizontalHeader()->isSectionHidden(i)) {
			enabledColumns.append(QString::number(i));
		}
	}

	settings->beginGroup(group);
	settings->setValue("tableHeaders", sizes);
	settings->setValue("tableHeadersEnabled", enabledColumns);
	settings->endGroup();
}

void ToggleColumnsTableView::loadGeometry(QSettings *settings, const QString &group, const QList<int> &defaultColumns)
{
	settings->beginGroup(group);
	const QStringList sizes = settings->value("tableHeaders").toStringList();
	const QStringList visibility = settings->value("tableHeadersEnabled").toStringList();
	settings->endGroup();

	for (int i = 0; i < model()->columnCount(); ++i) {
		const int size = i < sizes.size() ? sizes[i].toInt() : 0;
		const bool enabled = !visibility.isEmpty() ? visibility.contains(QString::number(i)) : defaultColumns.contains(i);

		horizontalHeader()->resizeSection(i, size > 0 ? size : 100);
		horizontalHeader()->setSectionHidden(i, !enabled);
	}
}


void ToggleColumnsTableView::headersContextMenu(const QPoint &pos)
{
	Q_UNUSED(pos);

	auto *menu = new QMenu(this);

	for (int i = 0; i < model()->columnCount(); ++i) {
		const bool enabled = !horizontalHeader()->isSectionHidden(i);
		const QString title = model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

		if (title.isEmpty()) {
			continue;
		}

		auto *action = menu->addAction(title, [this, i]() { toggleColumn(i); });
		action->setCheckable(true);
		action->setChecked(enabled);
	}

	menu->exec(QCursor::pos());
}

void ToggleColumnsTableView::toggleColumn(int index)
{
	const bool current = horizontalHeader()->isSectionHidden(index);
	horizontalHeader()->setSectionHidden(index, !current);
}
