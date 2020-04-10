#include "analytics.h"

class QString;
class QVariant;


void Analytics::setTrackingID(const QString& trackingId)
{
	m_googleAnalytics.setTrackingID(trackingId);
}

void Analytics::setEnabled(bool enabled)
{
    m_enabled = enabled;
}


void Analytics::startSending()
{
	m_googleAnalytics.startSending();
}

void Analytics::startSession()
{
	m_googleAnalytics.startSession();
}

void Analytics::endSession()
{
	m_googleAnalytics.endSession();
}

void Analytics::sendScreenView(const QString& screenName, const QVariantMap& customValues)
{
    if (!m_enabled) {
        return;
    }

	m_googleAnalytics.sendScreenView(screenName, customValues);
	m_googleAnalytics.startSending();
}

void Analytics::sendEvent(const QString& category, const QString& action, const QString &label, const QVariant &value, const QVariantMap &customValues)
{
    if (!m_enabled) {
        return;
    }

	m_googleAnalytics.sendEvent(category, action, label, value, customValues);
	m_googleAnalytics.startSending();
}
