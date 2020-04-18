#ifndef MONITOR_TABLE_MODEL_H
#define MONITOR_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>
#include "monitoring-center.h"


class MonitorManager;
class QModelIndex;

class MonitorTableModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		MonitorTableModel(MonitorManager *monitorManager, QObject *parent = nullptr);

		// Data
		int rowCount(const QModelIndex &parent = {}) const override;
		int columnCount(const QModelIndex &parent = {}) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

		// Changes
		bool insertRows(int position, int rows, const QModelIndex &parent) override;
		bool removeRows(int position, int rows, const QModelIndex &parent) override;

	public slots:
		bool setStatus(const Monitor &monitor, MonitoringCenter::MonitoringStatus status);

	protected slots:
		// Handle signals when the underlying data changes
		void inserted(int position);
		void removed(int position);

	private:
		MonitorManager *m_monitorManager;
		QMap<int, MonitoringCenter::MonitoringStatus> m_statuses;
		bool m_freeze = false;
};

#endif // MONITOR_TABLE_MODEL_H
