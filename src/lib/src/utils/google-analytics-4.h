#ifndef GOOGLE_ANALYTICS_4_H
#define GOOGLE_ANALYTICS_4_H

#include <QObject>
#include <QVariantMap>


class QNetworkAccessManager;

class GoogleAnalytics4 : public QObject
{
    Q_OBJECT

	Q_PROPERTY(QString apiSecret READ apiSecret WRITE setApiSecret NOTIFY apiSecretChanged)
	Q_PROPERTY(QString measurementId READ measurementId WRITE setMeasurementId NOTIFY measurementIdChanged)
	Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)

	public:
		explicit GoogleAnalytics4(QObject *parent = nullptr);

	public:
		void setApiSecret(const QString &apiSecret);
		QString apiSecret() const;
		void setMeasurementId(const QString &measurementId);
		QString measurementId() const;
		void setUserId(const QString &userId);
		QString userId() const;

	public slots:
		// Generic event
		void sendEvent(const QString &name, const QVariantMap &parameters = {});

		// Standard event

	signals:
		void apiSecretChanged();
		void measurementIdChanged();
		void userIdChanged();

	protected:
#ifdef QT_GUI_LIB
		QString screenResolution() const;
	#endif
		QString userAgent() const;
		QString userAgentPlatformVersion() const;

	private:
		QNetworkAccessManager *m_networkAccessManager;
		unsigned int m_sessionId;
		QString m_apiSecret;
		QString m_measurementId;
		QString m_clientId;
		QString m_userId;
};

#endif // GOOGLE_ANALYTICS_4_H
