#include "network-reply.h"
#include <utility>
#include "custom-network-access-manager.h"


NetworkReply::NetworkReply(QNetworkRequest request, CustomNetworkAccessManager *manager, QObject *parent)
	: QObject(parent), m_request(std::move(request)), m_manager(manager)
{
	init();
}

NetworkReply::NetworkReply(QNetworkRequest request, QByteArray data, CustomNetworkAccessManager *manager, QObject *parent)
	: QObject(parent), m_request(std::move(request)), m_data(std::move(data)), m_manager(manager), m_post(true)
{
	init();
}

void NetworkReply::init()
{
	timer.setSingleShot(true);
	connect(&timer, &QTimer::timeout, this, &NetworkReply::startNow);
}


QUrl NetworkReply::url() const
{
	if (m_reply != nullptr) {
		return m_reply->url();
	}
	return m_request.url();
}

QVariant NetworkReply::attribute(QNetworkRequest::Attribute code) const
{
	if (m_reply != nullptr) {
		return m_reply->attribute(code);
	}
	return QVariant();
}

QByteArray NetworkReply::readAll()
{
	return m_reply->readAll();
}

qint64 NetworkReply::bytesAvailable() const
{
	return m_reply->bytesAvailable();
}

QNetworkReply::NetworkError NetworkReply::error() const
{
	if (m_reply != nullptr) {
		return m_reply->error();
	}
	return QNetworkReply::NetworkError::NoError;
}

QString NetworkReply::errorString() const
{
	if (m_reply != nullptr) {
		return m_reply->errorString();
	}
	return QString();
}

QNetworkReply *NetworkReply::networkReply() const
{
	return m_reply;
}

bool NetworkReply::isRunning() const
{
	if (!m_started && !m_aborted) {
		return true;
	}
	return m_reply != nullptr && m_reply->isRunning();
}


void NetworkReply::start(int msDelay)
{
	if (m_started) {
		return;
	}
	if (m_aborted) {
		m_aborted = false;
		return;
	}
	m_started = true;

	timer.setInterval(msDelay);
	timer.start();
}

void NetworkReply::startNow()
{
	if (m_post) {
		m_reply = m_manager->post(m_request, m_data);
	} else {
		m_reply = m_manager->get(m_request);
	}

	connect(m_reply, &QNetworkReply::readyRead, this, &NetworkReply::readyRead);
	connect(m_reply, &QNetworkReply::downloadProgress, this, &NetworkReply::downloadProgress);
	connect(m_reply, &QNetworkReply::finished, this, &NetworkReply::finished);

	m_reply->setParent(this);
}

void NetworkReply::abort()
{
	m_aborted = true;
	if (m_reply != nullptr) {
		m_reply->abort();
	}
	if (timer.isActive()) {
		timer.stop();
	}
}
