#ifndef DOWNLOAD_GROUP_TABLE_MODEL_H
#define DOWNLOAD_GROUP_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include "downloader/download-query-group.h"


class QModelIndex;

class DownloadGroupTableModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		DownloadGroupTableModel(Profile *profile, QList<DownloadQueryGroup> &downloads, QWidget *parent = nullptr);
		const DownloadQueryGroup &dataForRow(int row);

		// Data
		int rowCount(const QModelIndex &parent = {}) const override;
		int columnCount(const QModelIndex &parent = {}) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

		// Edition
		Qt::ItemFlags flags(const QModelIndex &index) const override;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
		bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;

		// Drag & drop
		Qt::DropActions supportedDropActions() const override;
		bool insertRows(int row, int count, const QModelIndex &parent) override;
		bool removeRows(int row, int count, const QModelIndex &parent) override;
		bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

	public slots:
		// Handle signals when the underlying data changes
		void inserted(int position);
		void removed(int position);
		void changed(int position);
		void cleared();
		bool setStatus(const DownloadQueryGroup &download, int status);

	private:
		const Profile *m_profile;
		QList<DownloadQueryGroup> &m_downloads;
		QMap<int, int> m_statuses;
};

#endif // DOWNLOAD_GROUP_TABLE_MODEL_H
