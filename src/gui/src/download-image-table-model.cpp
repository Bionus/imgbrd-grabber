#include "download-image-table-model.h"
#include <QDateTime>
#include <QIcon>
#include <QStringList>
#include <QVariant>
#include <utility>
#include "functions.h"
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
	return 12;
}

QVariant DownloadImageTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			static QStringList headers { "Id", "Md5", "Rating", "Tags", "Url", "Date", "Search", "Site", "Filename", "Folder", "File size", "Dimensions" };
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
	const QSharedPointer<Image> &img = download.image;

	if (role != Qt::DisplayRole) {
		return {};
	}

	switch (index.column())
	{
		case 0: return QString::number(img->id());
		case 1: return img->md5();
		case 2: return img->token<QString>("rating");
		case 3: return img->tagsString().join(' ');
		case 4: return img->fileUrl().toString();
		case 5: return img->createdAt().toString(Qt::ISODate);
		case 6: return img->search().join(' ');
		case 7: return download.site->url();
		case 8: return download.filename;
		case 9: return download.path;

		case 10: {
			double size = img->fileSize();
			const QString unit = getUnit(&size);
			return size > 0
				? QStringLiteral("%1 %2").arg(size).arg(unit)
				: QString();
		}

		case 11:
			return img->width() > 0 && img->height() > 0
				? QStringLiteral("%1 x %2").arg(img->width()).arg(img->height())
				: QString();
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
