#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include "vendor/ganalytics.h"


class Analytics
{
    public:
		// Singleton pattern
        static Analytics &getInstance()
		{
			static Analytics instance;
			return instance;
		}
		Analytics(Analytics const &) = delete;
		void operator=(Analytics const &) = delete;

        // Setters
        void setTrackingID(const QString& trackingId);
        void setEnabled(bool enabled);

        // API
        void sendScreenView(const QString& screenName, const QVariantMap &customValues = {});
        void sendEvent(const QString& category, const QString &action, const QString &label = {}, const QVariant &value = {}, const QVariantMap &customValues = {});

    private:
		Analytics() = default;

        bool m_enabled = false;
        GAnalytics m_googleAnalytics;
};

#endif // ANALYTICS_H
