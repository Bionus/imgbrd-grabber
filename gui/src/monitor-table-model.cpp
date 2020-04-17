#include "monitor-table-model.h"
#include <QIcon>
#include <QtMath>
#include <QVariant>
#include <utility>
#include "monitoring-center.h"
#include "models/monitor.h"
#include "models/monitor-manager.h"
#include "models/site.h"


MonitorTableModel::MonitorTableModel(MonitorManager *monitorManager, QObject *parent)
	: QAbstractTableModel(parent), m_monitorManager(monitorManager)
{
	connect(m_monitorManager, &MonitorManager::inserted, this, &MonitorTableModel::inserted);
	connect(m_monitorManager, &MonitorManager::removed, this, &MonitorTableModel::removed);
}


int MonitorTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_monitorManager->monitors().count();
}

int MonitorTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 5;
}

QVariant MonitorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case 0: return QString("");
				case 1: return QString("Search");
				case 2: return QString("Source");
				case 3: return QString("Interval");
				case 4: return QString("Action");
			}
		} else {
			return QString::number(section + 1);
		}
	}
	return {};
}

QVariant MonitorTableModel::data(const QModelIndex &index, int role) const
{
	const Monitor &monitor = m_monitorManager->monitors()[index.row()];

	// Icon in the first column
	if (role == Qt::DecorationRole && index.column() == 0) {
		static QMap<MonitoringCenter::MonitoringStatus, QIcon> s_iconMap
		{
			{ MonitoringCenter::Waiting, QIcon(":/images/status/pending.png") },
			{ MonitoringCenter::Checking, QIcon(":/images/status/downloading.png") },
			{ MonitoringCenter::Performing, QIcon(":/images/status/ok.png") },
		};
		auto status = MonitoringCenter::Waiting; // FIXME
		if (role != Qt::DecorationRole || !s_iconMap.contains(status)) {
			return {};
		}
		return s_iconMap[status];
	}

	if (role != Qt::DisplayRole) {
		return {};
	}

	switch (index.column()) {
		case 1:
			return monitor.query().toString();

		case 2:
		{
			QStringList sites;
			for (auto site : monitor.sites()) {
				sites.append(site->url());
			}
			return sites.join(", ");
		}

		case 3:
		{
			const int interval = qFloor(monitor.interval() / 60.0);
			const int days = interval / 1440;
			const int mins = interval % 1440;
			const QString timeFormat = mins > 60 ? (mins % 60 != 0 ? tr("h 'h' m 'm'") : tr("h 'h'")) : tr("m 'm'");
			QString sDate = days > 0 ? QString("%1 d ").arg(days) : "";
			QString sTime = mins > 0 ? QTime(0, 0, 0).addSecs(mins * 60).toString(timeFormat) : "";
			return sDate + sTime;
		}

		case 4:
		{
			QStringList actions;
			if (monitor.notify()) {
				actions.append("Notify");
			}
			if (monitor.download()) {
				actions.append("Download");
			}
			return actions.join(", ");
		}
	}

	return {};
}


void MonitorTableModel::inserted(int position)
{
	if (!m_freeze) {
		beginInsertRows(QModelIndex(), position, position);
		endInsertRows();
	}
}

bool MonitorTableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent);

	m_freeze = true;
	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		m_monitorManager->add(Monitor(QList<Site*>(), 24 * 60 * 60, QDateTime::currentDateTime(), false, QString(), QString()), position);
	}

	endInsertRows();
	m_freeze = false;

	return true;
}

void MonitorTableModel::removed(int position)
{
	if (!m_freeze) {
		beginRemoveRows(QModelIndex(), position, position);
		endRemoveRows();
	}
}

bool MonitorTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent);

	m_freeze = true;
	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		m_monitorManager->remove(m_monitorManager->monitors()[position]);
	}

	endRemoveRows();
	m_freeze = false;

	return true;
}
