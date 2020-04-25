#include "download-image-table-model.h"
#include <QDateTime>
#include <QIcon>
#include <QStringList>
#include <QVariant>
#include <utility>
#include "models/image.h"
#include "models/site.h"


DownloadImageTableModel::DownloadImageTableModel(QList<DownloadQueryImage> &downloads, QObject *parent)
	: QAbstractTableModel(parent), m_downloads(downloads)
{}


int DownloadImageTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_downloads.count();
}

int DownloadImageTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 10;
}

QVariant DownloadImageTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			static QStringList headers { "Id", "Md5", "Rating", "Tags", "Url", "Date", "Search", "Site", "Filename", "Folder" };
			return headers[section];
		} else {
			return QString::number(section + 1);
		}
	}
	return {};
}

QVariant DownloadImageTableModel::data(const QModelIndex &index, int role) const
{
	const DownloadQueryImage &download = m_downloads[index.row()];

	if (role != Qt::DisplayRole) {
		return {};
	}

	switch (index.column()) {
		case 0: return QString::number(download.image->id());
		case 1: return download.image->md5();
		case 2: return download.image->token<QString>("rating");
		case 3: return download.image->tagsString().join(' ');
		case 4: return download.image->fileUrl().toString();
		case 5: return download.image->createdAt().toString(Qt::ISODate);
		case 6: return download.image->search().join(' ');
		case 7: return download.site->url();
		case 8: return download.filename;
		case 9: return download.path;
	}

	return {};
}


void DownloadImageTableModel::inserted(int position)
{
	beginInsertRows(QModelIndex(), position, position);
	endInsertRows();
}

void DownloadImageTableModel::removed(int position)
{
	beginRemoveRows(QModelIndex(), position, position);
	endRemoveRows();
}

void DownloadImageTableModel::changed(int position)
{
	auto topLeft = index(position, 0);
	auto bottomRight = index(position, columnCount() - 1);
	emit dataChanged(topLeft, bottomRight);
}

void DownloadImageTableModel::cleared()
{
	beginResetModel();
	endResetModel();
}
