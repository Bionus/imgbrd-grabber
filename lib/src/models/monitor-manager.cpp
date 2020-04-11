#include "models/monitor-manager.h"
#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <utility>
#include "models/monitor.h"


MonitorManager::MonitorManager(QString file, const QMap<QString, Site*> &sites)
	: m_file(std::move(file)), m_sites(sites)
{
	load();
}

void MonitorManager::load()
{
	QFile file(m_file);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return;
	}

	const QByteArray data = file.readAll();
	QJsonDocument loadDoc = QJsonDocument::fromJson(data);
	QJsonObject object = loadDoc.object();

	QJsonArray monitors = object["monitors"].toArray();
	for (const auto &monitorJson : monitors) {
		m_monitors.append(Monitor::fromJson(monitorJson.toObject(), m_sites));
	}
}

void MonitorManager::save() const
{
	QFile file(m_file);
	if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		return;
	}

	// Generate JSON array
	QJsonArray monitorsJson;
	for (const Monitor &monitor : m_monitors) {
		QJsonObject unique;
		monitor.toJson(unique);
		monitorsJson.append(unique);
	}

	// Generate result
	QJsonObject full;
	full["version"] = 1;
	full["monitors"] = monitorsJson;

	// Write result
	QJsonDocument saveDoc(full);
	file.write(saveDoc.toJson());
	file.close();
}

void MonitorManager::add(const Monitor &monitor)
{
	m_monitors.append(monitor);

	save();
	emit changed();
}

void MonitorManager::remove(const Monitor &monitor)
{
	m_monitors.removeAll(monitor);

	save();
	emit changed();
}

const QList<Monitor> &MonitorManager::monitors() const
{
	return m_monitors;
}
