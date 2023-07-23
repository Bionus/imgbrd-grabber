#include "qt-google-analytics.h"
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QOperatingSystemVersion>
#include "chromium-user-agent.h"

#ifdef QT_GUI_LIB
	#include <QGuiApplication>
	#include <QScreen>
#endif
#ifdef Q_OS_ANDROID
	#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		#include <QJniObject>
	#else
		#include <QAndroidJniObject>
		typedef QAndroidJniObject QJniObject;
	#endif
#endif

#define MEASUREMENT_ENDPOINT_WEB "https://www.google-analytics.com/g/collect"
#define CLIENT_ID_SETTINGS_KEY "QtGoogleAnalytics/ClientId"
#define SESSION_START_INTERVAL_SECONDS 1800
#define USER_AGENT_PRODUCT "Chrome/114.0.0.0"


#include "functions.h"
QtGoogleAnalytics::QtGoogleAnalytics(QObject *parent)
	: QObject(parent)
{
	m_networkAccessManager = new QNetworkAccessManager(this);
	m_sessionId = QDateTime::currentSecsSinceEpoch();

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	if (!settings.contains(CLIENT_ID_SETTINGS_KEY)) {
		m_clientId = QString("%1.%2").arg(QString::number(QRandomGenerator::global()->generate()), QString::number(QDateTime::currentSecsSinceEpoch()));
		settings.setValue(CLIENT_ID_SETTINGS_KEY, m_clientId);
		m_isFirstVisit = true;
	} else {
		m_clientId = settings.value(CLIENT_ID_SETTINGS_KEY).toString();
	}

	m_userAgent = userAgent();
}

QtGoogleAnalytics::QtGoogleAnalytics(const QString &measurementId, QObject *parent)
	: QtGoogleAnalytics(parent)
{
	m_measurementId = measurementId;
}


void QtGoogleAnalytics::setMeasurementId(const QString &measurementId)
{
	if (m_measurementId != measurementId) {
		m_measurementId = measurementId;
		emit measurementIdChanged();
	}
}

QString QtGoogleAnalytics::measurementId() const
{
	return m_measurementId;
}

void QtGoogleAnalytics::setUserId(const QString &userId)
{
	if (m_userId != userId) {
		m_userId = userId;
		emit userIdChanged();
	}
}

QString QtGoogleAnalytics::userId() const
{
	return m_userId;
}

void QtGoogleAnalytics::setUserProperties(const QVariantMap &userProperties)
{
	if (m_userProperties != userProperties) {
		m_userProperties = userProperties;
		emit userPropertiesChanged();
	}
}

QVariantMap QtGoogleAnalytics::userProperties() const
{
	return m_userProperties;
}

void QtGoogleAnalytics::setDebugModeEnabled(bool debugModeEnabled)
{
	if (m_debugModeEnabled != debugModeEnabled) {
		m_debugModeEnabled = debugModeEnabled;
		emit debugModeEnabledChanged();
	}
}

bool QtGoogleAnalytics::debugModeEnabled() const
{
	return m_debugModeEnabled;
}


void QtGoogleAnalytics::sendEvent(const QString &name, const QVariantMap &parameters)
{
	QUrl url(MEASUREMENT_ENDPOINT_WEB);

	QUrlQuery query {
		{ "v", "2" },
		{ "tid", m_measurementId },
		{ "_p", QString::number(QRandomGenerator::global()->generate()) },
		{ "cid", m_clientId },
		{ "ul", QLocale::system().name().toLower().replace("_", "-") },
		#ifdef QT_GUI_LIB
			{ "sr", screenResolution() },
		#endif
		{ "_s", "1" },
		{ "sid", QString::number(m_sessionId) },
		{ "sct", "1" },
		{ "_et", "1" }, // Necessary for users to be created
		{ "en", name },

		// Operating System information
		{ "uaa", m_uach.arch() },
		{ "uab", m_uach.bitness() },
		{ "uamb", m_uach.mobile() ? "1" : "0" },
		{ "uam", m_uach.model() },
		{ "uap", m_uach.platform() },
		{ "uapv", m_uach.platformVersion() },
		{ "uaw", m_uach.wow64() ? "1" : "0" },
	};
	if (!m_userId.isEmpty()) {
		query.addQueryItem("uid", m_userId);
	}
	if (m_debugModeEnabled) {
		query.addQueryItem("_dbg", "1");
	}
	if (!m_lastEvent.isValid()) {
		query.addQueryItem("_nsi", "1");
	}
	if (!m_lastEvent.isValid() || m_lastEvent.secsTo(QDateTime::currentDateTimeUtc()) > SESSION_START_INTERVAL_SECONDS) {
		query.addQueryItem("_ss", "1");
	}
	if (m_isFirstVisit) {
		query.addQueryItem("_fv", "1");
	}

	// Events
	for (auto it = parameters.constBegin(); it != parameters.constEnd(); ++it) {
		if (it.value().type() == QVariant::Type::Int) {
			query.addQueryItem("epn." + it.key(), QString::number(it.value().toInt()));
		} else {
			query.addQueryItem("ep." + it.key(), it.value().toString());
		}
	}

	// User properties
	for (auto it = m_userProperties.constBegin(); it != m_userProperties.constEnd(); ++it) {
		if (it.value().type() == QVariant::Type::Int) {
			query.addQueryItem("upn." + it.key(), QString::number(it.value().toInt()));
		} else {
			query.addQueryItem("up." + it.key(), it.value().toString());
		}
	}

	url.setQuery(query);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setHeader(QNetworkRequest::UserAgentHeader, m_userAgent);
	m_uach.setRequestHeaders(request);

	QNetworkReply *reply = m_networkAccessManager->post(request, QByteArray());
	connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);

	m_lastEvent = QDateTime::currentDateTimeUtc();
	m_isFirstVisit = false;
}


#ifdef QT_GUI_LIB
	QString QtGoogleAnalytics::screenResolution() const
	{
		const QScreen *screen = QGuiApplication::primaryScreen();
		if (screen == nullptr) {
			return {};
		}

		const QSize size = screen->size();
		return QString("%1x%2").arg(size.width()).arg(size.height());
	}
#endif

QString QtGoogleAnalytics::userAgent() const
{
	#if defined(Q_OS_ANDROID)
		// On Android, just use System.getProperty("http.agent")
		return QJniObject::callStaticObjectMethod(
			"System",
			"getProperty",
			"(Ljava/lang/String;)Z",
			QJniObject::fromString("http.agent").object<jstring>()
		).toString();
	#endif

	// On other platforms, use a Chrome User-Agent
	return buildUserAgentForProduct(USER_AGENT_PRODUCT);
}
