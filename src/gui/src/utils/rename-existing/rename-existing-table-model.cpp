#include "utils/rename-existing/rename-existing-table-model.h"
#include <QColor>
#include <QPixmap>
#include <QVariant>


RenameExistingTableModel::RenameExistingTableModel(const QList<RenameExistingFile> &files, QString folder, QObject *parent)
	: QAbstractTableModel(parent), m_files(files), m_folder(folder)
{}

int RenameExistingTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_files.count();
}

int RenameExistingTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 3;
}

QVariant RenameExistingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch (section)
		{
			case 0: return QString("Thumbnail");
			case 1: return QString("Original");
			case 2: return QString("Destination");
		}
	}
	return {};
}

QVariant RenameExistingTableModel::data(const QModelIndex &index, int role) const
{
	const RenameExistingFile &image = m_files[index.row()];

	// Preview in the first column
	if (index.column() == 0 && role == Qt::DecorationRole) {
		return QPixmap(image.path).scaledToHeight(50, Qt::SmoothTransformation);
	}

	// Original path in the second column
	if (index.column() == 1 && role == Qt::DisplayRole) {
		return image.path.right(image.path.length() - m_folder.length() - 1);
	}

	// New path in the third column
	if (index.column() == 2) {
		// "no change" in red if there isn't
		if (image.path == image.newPath) {
			if (role == Qt::ForegroundRole) {
				return QVariant::fromValue(QColor(Qt::red));
			}
			if (role == Qt::DisplayRole) {
				return QString("No change");
			}
		}

		if (role == Qt::DisplayRole) {
			return image.newPath.right(image.newPath.length() - m_folder.length() - 1);
		}
	}

	return {};
}
