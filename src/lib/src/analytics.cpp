#include "analytics.h"
#include <QCoreApplication>

class QString;
class QVariant;


void Analytics::setTrackingID(const QString& trackingId)
{
	m_googleAnalytics.setTrackingID(trackingId);
}

void Analytics::setMeasurementID(const QString& measurementId)
{
	m_ga4.setMeasurementId(measurementId);
}

void Analytics::setEnabled(bool enabled)
{
	m_enabled = enabled;
	if (!enabled) {
		m_googleAnalytics.stopSending();
	}
}


void Analytics::startSending()
{
	if (!m_enabled) {
		return;
	}

	m_googleAnalytics.startSending();
}

void Analytics::startSession()
{
	if (!m_enabled) {
		return;
	}

	m_googleAnalytics.startSession();
}

void Analytics::endSession()
{
	if (!m_enabled) {
		return;
	}

	m_googleAnalytics.endSession();
}

void Analytics::sendScreenView(const QString& screenName, const QVariantMap& customValues)
{
	if (!m_enabled) {
		return;
	}

	m_googleAnalytics.sendScreenView(screenName, customValues);
	m_googleAnalytics.startSending();

	QVariantMap eventParams(customValues);
	eventParams["firebase_screen"] = screenName;
	eventParams["firebase_screen_class"] = screenName;
	eventParams["app_name"] = qApp->applicationName();
	eventParams["app_version"] = qApp->applicationVersion();
	m_ga4.sendEvent("screen_view", eventParams);
}

void Analytics::sendEvent(const QString& category, const QString& action, const QString &label, const QVariant &value, const QVariantMap &customValues)
{
	if (!m_enabled) {
		return;
	}

	m_googleAnalytics.sendEvent(category, action, label, value, customValues);
	m_googleAnalytics.startSending();

	QVariantMap eventParams(customValues);
	if (!label.isEmpty()) {
		eventParams["label"] = label;
	}
	if (!value.isNull()) {
		eventParams["value"] = value;
	}
	m_ga4.sendEvent(QString("%1/%2").arg(category, action), eventParams);
}
