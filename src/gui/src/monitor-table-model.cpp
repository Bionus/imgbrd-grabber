#include "monitor-table-model.h"
#include <QIcon>
#include <QStringList>
#include <QtMath>
#include <QVariant>
#include <utility>
#include "models/site.h"
#include "monitoring/monitoring-center.h"
#include "monitoring/monitor.h"
#include "monitoring/monitor-manager.h"


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
	return 12;
}

QVariant MonitorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section)
			{
				case 0: return QString();
				case 1: return tr("Search");
				case 2: return tr("Source");
				case 3: return tr("Interval");
				case 4: return tr("Action");
				case 5: return tr("Post-filters");
				case 6: return tr("Last check");
				case 7: return tr("Next check");
				case 8: return tr("Last state");
				case 9: return tr("Last state count");
				case 10: return tr("Last state since");
				case 11: return tr("Last success");
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
	if (role == Qt::DecorationRole) {
		switch (index.column())
		{
			case 0: {
				static const QMap<MonitoringCenter::MonitoringStatus, QIcon> s_statusIconMap
				{
					{MonitoringCenter::Waiting, QIcon(":/images/status/pending.png")},
					{MonitoringCenter::Checking, QIcon(":/images/status/downloading.png")},
					{MonitoringCenter::Performing, QIcon(":/images/status/ok.png")},
				};
				const auto status = m_statuses.contains(row) ? m_statuses[row] : MonitoringCenter::Waiting;
				if (!s_statusIconMap.contains(status)) {
					return {};
				}
				return s_statusIconMap[status];
			}

			case 8: {
				static const QMap<QString, QIcon> s_lastStateIconMap
				{
					{"empty", QIcon(":/images/status/error.png")},
					{"finished", QIcon(":/images/status/ignored.png")},
					{"ok", QIcon(":/images/status/ok.png")},
					{"mixed", QIcon(":/images/status/unknown.png")},
				};
				const QString lastState = monitor.lastState();
				if (!s_lastStateIconMap.contains(lastState)) {
					return {};
				}
				return s_lastStateIconMap[lastState];
			}
		}
		return {};
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
			return monitor.lastCheck();

		case 7:
			return timeToString(monitor.secsToNextCheck());

		case 8:
			return monitor.lastState();

		case 9:
			if (monitor.lastState().isEmpty()) {
				return {};
			}
			return tr("%n time(s)", "", monitor.lastStateCount());

		case 10:
			if (monitor.lastState().isEmpty()) {
				return {};
			}
			return monitor.lastStateSince();

		case 11:
			return monitor.lastSuccess();
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
		m_monitorManager->add(Monitor(QList<Site*>(), 24 * 60 * 60, QDateTime::currentDateTimeUtc(), QDateTime::currentDateTime(), false, QString(), QString()), position);
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
