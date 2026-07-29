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

NetworkReply::~NetworkReply()
{
	abort();

	if (m_reply != nullptr) {
		m_reply->deleteLater();
		m_reply = nullptr;
	}
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
	// An aborted reply is closed but not null. Reading it yields nothing and logs
	// "QIODevice::read (QNetworkReplyHttpImpl): device not open" for every attempt.
	if (m_reply != nullptr && m_reply->isOpen()) {
		return m_reply->readAll();
	}
	return {};
}

qint64 NetworkReply::bytesAvailable() const
{
	if (m_reply != nullptr) {
		return m_reply->bytesAvailable();
	}
	return 0;
}

QNetworkReply::NetworkError NetworkReply::error() const
{
	if (m_reply != nullptr) {
		return m_reply->error();
	}
	if (m_aborted) {
		return QNetworkReply::NetworkError::OperationCanceledError;
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

QByteArray NetworkReply::rawHeader(const QByteArray &headerName) const
{
	if (m_reply != nullptr) {
		return m_reply->rawHeader(headerName);
	}
	return {};
}


bool NetworkReply::isRunning() const
{
	if (m_finished || m_aborted) {
		return false;
	}

	// A reply that hasn't been actually started yet is considered as running
	if (!m_started) {
		return true;
	}

	return m_reply != nullptr && m_reply->isRunning();
}


void NetworkReply::start(int msDelay)
{
	if (m_started || m_finished) {
		return;
	}

	// Stay aborted. This used to clear m_aborted and return without starting, which
	// left the reply reporting isRunning() forever: no request in flight, no timer
	// pending, and no signal it was ever going to emit.
	if (m_aborted) {
		return;
	}

	m_started = true;

	timer.setInterval(msDelay);
	timer.start();
}

// Single funnel for completion, so finished() is emitted at most once per reply
void NetworkReply::finishNow()
{
	if (m_finished) {
		return;
	}
	m_finished = true;
	emit finished();
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
	connect(m_reply, &QNetworkReply::redirected, this, &NetworkReply::redirected);
	connect(m_reply, &QNetworkReply::finished, this, &NetworkReply::finishNow);

	m_reply->setParent(this);
}

void NetworkReply::abort()
{
	// Do nothing if we are already aborted or already done
	if (m_aborted || m_finished) {
		return;
	}

	m_aborted = true;
	if (timer.isActive()) {
		timer.stop();
	}

	if (m_reply != nullptr) {
		// Qt emits QNetworkReply::finished() for us, which is already connected
		if (m_reply->isRunning()) {
			m_reply->abort();
		}
		return;
	}

	// Never dispatched. Deliberately emit nothing: callers that abort() do not want a
	// completion callback. NetworkManager releases our concurrency slot on destroyed().
}
