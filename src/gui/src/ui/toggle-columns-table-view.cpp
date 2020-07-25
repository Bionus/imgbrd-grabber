#include "ui/toggle-columns-table-view.h"
#include <QHeaderView>
#include <QMenu>
#include <QSettings>


ToggleColumnsTableView::ToggleColumnsTableView(QWidget *parent)
	: QTableView(parent)
{
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &ToggleColumnsTableView::headersContextMenu);

	horizontalHeader()->setSectionsMovable(true);
}


void ToggleColumnsTableView::saveGeometry(QSettings *settings, const QString &group)
{
	settings->beginGroup(group);
	settings->setValue("headerState", horizontalHeader()->saveState());
	settings->endGroup();
}

void ToggleColumnsTableView::loadGeometry(QSettings *settings, const QString &group, const QList<int> &defaultColumns)
{
	settings->beginGroup(group);

	const auto state = settings->value("headerState").toByteArray();
	if (!state.isEmpty()) {
		horizontalHeader()->restoreState(state);
	} else {
		for (int i = 0; i < model()->columnCount(); ++i) {
			if (defaultColumns.isEmpty() || !defaultColumns.contains(i)) {
				horizontalHeader()->setSectionHidden(i, true);
			}
		}
	}

	settings->endGroup();
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
