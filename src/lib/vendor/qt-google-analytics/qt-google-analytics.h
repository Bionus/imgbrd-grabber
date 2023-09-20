#ifndef QT_GOOGLE_ANALYTICS_H
#define QT_GOOGLE_ANALYTICS_H

#include <QObject>
#include <QDateTime>
#include <QVariantMap>
#include "user-agent-client-hints.h"


class QNetworkAccessManager;

class QtGoogleAnalytics : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString measurementId READ measurementId WRITE setMeasurementId NOTIFY measurementIdChanged)
	Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
	Q_PROPERTY(QVariantMap userProperties READ userProperties WRITE setUserProperties NOTIFY userPropertiesChanged)
	Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)
	Q_PROPERTY(bool debugModeEnabled READ debugModeEnabled WRITE setDebugModeEnabled NOTIFY debugModeEnabledChanged)

	public:
		explicit QtGoogleAnalytics(QObject *parent = nullptr);
		explicit QtGoogleAnalytics(const QString &measurementId, QObject *parent = nullptr);

	public:
		void setMeasurementId(const QString &measurementId);
		QString measurementId() const;
		void setUserId(const QString &userId);
		QString userId() const;
		void setUserProperties(const QVariantMap &userProperties);
		QVariantMap userProperties() const;
		void setUserAgent(const QString &userAgent);
		QString userAgent() const;
		void setDebugModeEnabled(bool debugModeEnabled);
		bool debugModeEnabled() const;

	public slots:
		void sendEvent(const QString &name, const QVariantMap &parameters = {});

	signals:
		void measurementIdChanged();
		void userIdChanged();
		void userPropertiesChanged();
		void userAgentChanged();
		void debugModeEnabledChanged();

	protected:
		#ifdef QT_GUI_LIB
			QString screenResolution() const;
		#endif
		QString generateUserAgent() const;

	private:
		QNetworkAccessManager *m_networkAccessManager;
		UserAgentClientHints m_uach;
		QString m_generatedUserAgent;

		unsigned int m_sessionId;
		QDateTime m_lastEvent;
		bool m_isFirstVisit = false;

		QString m_measurementId;
		QString m_clientId;
		QString m_userId;
		QVariantMap m_userProperties;
		QString m_userAgent;
		bool m_debugModeEnabled = false;
};

#endif // QT_GOOGLE_ANALYTICS_H
