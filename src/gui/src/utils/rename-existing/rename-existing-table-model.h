#ifndef RENAME_EXISTING_TABLE_MODEL_H
#define RENAME_EXISTING_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QVariant>
#include "rename-existing-file.h"


class RenameExistingTableModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		explicit RenameExistingTableModel(const QList<RenameExistingFile> &files, QString folder, QObject *parent = nullptr);

		// Data
		int rowCount(const QModelIndex &parent = {}) const override;
		int columnCount(const QModelIndex &parent = {}) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	private:
		const QList<RenameExistingFile> &m_files;
		const QString m_folder;
};

#endif // RENAME_EXISTING_TABLE_MODEL_H
