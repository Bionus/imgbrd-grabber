#include "download-group-table-model.h"
#include <QIcon>
#include <QStringList>
#include <QVariant>
#include <QWidget>
#include <utility>
#include "helpers.h"
#include "models/profile.h"
#include "models/site.h"


DownloadGroupTableModel::DownloadGroupTableModel(Profile *profile, QList<DownloadQueryGroup> &downloads, QWidget *parent)
	: QAbstractTableModel(parent), m_profile(profile), m_downloads(downloads)
{}

const DownloadQueryGroup &DownloadGroupTableModel::dataForRow(int row)
{
	return m_downloads[row];
}


int DownloadGroupTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_downloads.count();
}

int DownloadGroupTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 12;
}

QVariant DownloadGroupTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			static QStringList headers { "", "Tags", "Source", "Page", "Images per page", "Images limit", "Filename", "Folder", "Post-filtering", "Get blacklisted", "Galleries count as one", "Progress" };
			return headers[section];
		} else {
			return QString::number(section + 1);
		}
	}
	return {};
}

QVariant DownloadGroupTableModel::data(const QModelIndex &index, int role) const
{
	const int row = index.row();
	const DownloadQueryGroup &download = m_downloads[row];

	// Icon in the first column
	if (role == Qt::DecorationRole && index.column() == 0) {
		static QList<QIcon> s_iconMap
		{
			QIcon(":/images/status/pending.png"),
			QIcon(":/images/status/downloading.png"),
			QIcon(":/images/status/ok.png"),
		};
		int status = m_statuses.contains(row) ? m_statuses[row] : (download.progressVal <= 0 ? 0 : (download.progressVal >= download.total ? 2 : 1));
		if (status >= s_iconMap.count()) {
			return {};
		}
		return s_iconMap[status];
	}

	if (role != Qt::DisplayRole && role != Qt::EditRole) {
		return {};
	}

	switch (index.column())
	{
		case 1: return download.query.toString();
		case 2: return download.site->url();
		case 3: return QString::number(download.page);
		case 4: return QString::number(download.perpage);
		case 5: return QString::number(download.total);
		case 6: return download.filename;
		case 7: return download.path;
		case 8: return download.postFiltering.join(' ');
		case 9: return download.getBlacklisted ? "true" : "false";
		case 10: return download.galleriesCountAsOne ? "true" : "false";
		// 11th column is managed by a delegate
	}

	return {};
}


Qt::ItemFlags DownloadGroupTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	flags |= Qt::ItemIsDragEnabled;

	const int minColumn = m_downloads[index.row()].query.gallery.isNull() ? 1 : 2; // Cannot edit gallery queries
	if (index.column() >= minColumn && index.column() <= 10) {
		flags |= Qt::ItemIsEditable;
	}

	return flags;
}

bool DownloadGroupTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() || role != Qt::EditRole) {
		return false;
	}

	DownloadQueryGroup &download = m_downloads[index.row()];

	download.progressVal = 0;
	download.progressFinished = false;

	const QString val = value.toString();
	bool isInt = false;
	const int toInt = val.toInt(&isInt);

	switch (index.column())
	{
		case 1:
			if (download.query.gallery.isNull()) {
				download.query.tags = val.split(' ', Qt::SkipEmptyParts);
			}
			break;

		case 2:
			if (!m_profile->getSites().contains(val)) {
				error(qobject_cast<QWidget*>(parent()), tr("This source is not valid."));
				return false;
			}
			download.site = m_profile->getSites().value(val);
			break;

		case 3:
			download.page = toInt;
			break;

		case 4:
			if (toInt < 1 || !isInt) {
				error(qobject_cast<QWidget*>(parent()), tr("The image per page value must be greater or equal to 1."));
				return false;
			}
			download.perpage = toInt;
			break;

		case 5:
			if (toInt < 0 || !isInt) {
				error(qobject_cast<QWidget*>(parent()), tr("The image limit must be greater or equal to 0."));
				return false;
			}
			download.total = toInt;
			break;

		case 6:
			download.filename = val;
			break;

		case 7:
			download.path = val;
			break;

		case 8:
			download.postFiltering = val.split(' ', Qt::SkipEmptyParts);
			break;

		case 9:
			download.getBlacklisted = (val != "false");
			break;

		case 10:
			download.galleriesCountAsOne = (val != "false");
			break;
	}

	emit dataChanged(index, index, { role });
	return true;
}

bool DownloadGroupTableModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
	if (!roles.contains(Qt::EditRole) && !roles.contains(Qt::DisplayRole)) {
		return false;
	}
	return setData(index, roles[Qt::DisplayRole]);
}


Qt::DropActions DownloadGroupTableModel::supportedDropActions() const
{
	return Qt::MoveAction | Qt::CopyAction;
}

bool DownloadGroupTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
	Q_UNUSED(parent);
	beginInsertRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i < count; ++i) {
		m_downloads.insert(row, DownloadQueryGroup(m_profile->getSettings(), QStringList(), 1, 10, 10, QStringList(), m_profile->getSites().first()));
	}
	endInsertRows();
	return true;
}

bool DownloadGroupTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
	Q_UNUSED(parent);
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i < count; ++i) {
		m_downloads.removeAt(row);
	}
	endRemoveRows();
	return true;
}

bool DownloadGroupTableModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
	Q_UNUSED(sourceParent);
	Q_UNUSED(destinationParent);

	beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);

	for (int i = 0; i < count; ++i) {
		m_downloads.insert(destinationChild + i, m_downloads[sourceRow]);
		int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow + 1;
		m_downloads.removeAt(removeIndex);
	}

	endMoveRows();
	return true;
}

bool DownloadGroupTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column);

	if (row == -1) {
		row = rowCount();
	}

	return QAbstractTableModel::dropMimeData(data, action, row, 0, parent);
}


void DownloadGroupTableModel::inserted(int position)
{
	beginInsertRows(QModelIndex(), position, position);
	endInsertRows();
}

void DownloadGroupTableModel::removed(int position)
{
	beginRemoveRows(QModelIndex(), position, position);
	endRemoveRows();
}

void DownloadGroupTableModel::changed(int position)
{
	auto topLeft = index(position, 0);
	auto bottomRight = index(position, columnCount() - 1);
	emit dataChanged(topLeft, bottomRight);
}

void DownloadGroupTableModel::cleared()
{
	beginResetModel();
	endResetModel();
}

bool DownloadGroupTableModel::setStatus(const DownloadQueryGroup &download, int status)
{
	int position = m_downloads.indexOf(download);
	if (position < 0) {
		return false;
	}

	m_statuses[position] = status;
	changed(position);

	return true;
}
