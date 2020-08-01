#include "monitor-table-model.h"
#include <QIcon>
#include <QStringList>
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

	connect(&m_refreshTimer, &QTimer::timeout, [=]() {
		emit dataChanged(index(0, 6), index(rowCount() - 1, 7));
	});
	m_refreshTimer.start(60 * 1000);
}


int MonitorTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_monitorManager->monitors().count();
}

int MonitorTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 8;
}

QVariant MonitorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section)
			{
				case 0: return QString("");
				case 1: return QString("Search");
				case 2: return QString("Source");
				case 3: return QString("Interval");
				case 4: return QString("Action");
				case 5: return QString("Post-filters");
				case 6: return QString("Last check");
				case 7: return QString("Next check");
			}
		} else {
			return QString::number(section + 1);
		}
	}
	return {};
}

QString timeToString(int secs)
{
	if (secs >= 0 && secs < 60) {
		return QString("%1 s").arg(secs);
	}

	const int interval = qFloor(secs / 60.0);
	const int days = interval / 1440;
	const int mins = interval % 1440;

	const QString timeFormat = mins >= 60 ? (mins % 60 != 0 ? QObject::tr("h 'h' m 'm'") : QObject::tr("h 'h'")) : QObject::tr("m 'm'");
	QString sDate = days > 0 ? QString("%1 d ").arg(days) : "";
	QString sTime = mins > 0 ? QTime(0, 0, 0).addSecs(mins * 60).toString(timeFormat) : "";

	return sDate + sTime;
}

QVariant MonitorTableModel::data(const QModelIndex &index, int role) const
{
	const int row = index.row();
	const Monitor &monitor = m_monitorManager->monitors()[row];

	// Icon in the first column
	if (role == Qt::DecorationRole && index.column() == 0) {
		static const QMap<MonitoringCenter::MonitoringStatus, QIcon> s_iconMap
		{
			{ MonitoringCenter::Waiting, QIcon(":/images/status/pending.png") },
			{ MonitoringCenter::Checking, QIcon(":/images/status/downloading.png") },
			{ MonitoringCenter::Performing, QIcon(":/images/status/ok.png") },
		};
		auto status = m_statuses.contains(row) ? m_statuses[row] : MonitoringCenter::Waiting;
		if (!s_iconMap.contains(status)) {
			return {};
		}
		return s_iconMap[status];
	}

	if (role != Qt::DisplayRole) {
		return {};
	}

	switch (index.column())
	{
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
			return timeToString(monitor.interval());

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

		case 5:
			return monitor.postFilters().join(' ');

		case 6:
			return monitor.lastCheck().toString(Qt::DefaultLocaleShortDate);

		case 7:
			return timeToString(monitor.secsToNextCheck());
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

bool MonitorTableModel::setStatus(const Monitor &monitor, MonitoringCenter::MonitoringStatus status)
{
	int position = m_monitorManager->monitors().indexOf(monitor);
	if (position < 0) {
		return false;
	}

	m_statuses[position] = status;

	auto index = this->index(position, 0);
	emit dataChanged(index, index);

	return true;
}
