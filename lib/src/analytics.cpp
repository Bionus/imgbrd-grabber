#include "analytics.h"


void Analytics::setTrackingID(QString trackingId)
{
	m_googleAnalytics.setTrackingID(trackingId);
}

void Analytics::setEnabled(bool enabled)
{
    m_enabled = enabled;
}


void Analytics::sendScreenView(QString screenName, QVariantMap customValues)
{
    if (!m_enabled) {
        return;
    }

	m_googleAnalytics.sendScreenView(std::move(screenName), std::move(customValues));
	m_googleAnalytics.startSending();
}

void Analytics::sendEvent(QString category, QString action, QString label, QVariant value, QVariantMap customValues)
{
    if (!m_enabled) {
        return;
    }

	m_googleAnalytics.sendEvent(std::move(category), std::move(action), std::move(label), std::move(value), std::move(customValues));
	m_googleAnalytics.startSending();
}
