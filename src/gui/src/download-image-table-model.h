#ifndef DOWNLOAD_IMAGE_TABLE_MODEL_H
#define DOWNLOAD_IMAGE_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "downloader/download-query-image.h"


class QModelIndex;

class DownloadImageTableModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		DownloadImageTableModel(QList<DownloadQueryImage> &downloads, QObject *parent = nullptr);

		// Data
		int rowCount(const QModelIndex &parent = {}) const override;
		int columnCount(const QModelIndex &parent = {}) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	public slots:
		// Handle signals when the underlying data changes
		void inserted(int position);
		void removed(int position);
		void changed(int position);
		void cleared();

	private:
		QList<DownloadQueryImage> &m_downloads;
};

#endif // DOWNLOAD_IMAGE_TABLE_MODEL_H
