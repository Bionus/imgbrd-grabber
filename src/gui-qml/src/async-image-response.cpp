#include "async-image-response.h"
#include <QImage>
#include <QNetworkReply>
#include <QQuickTextureFactory>
#include <QTimer>


AsyncImageResponse::AsyncImageResponse(QNetworkReply *reply, const QRect &rect)
	: m_reply(reply), m_rect(rect)
{
	if (m_reply != nullptr) {
		connect(m_reply, &QNetworkReply::finished, this, &AsyncImageResponse::replyFinished);
	} else {
		m_error = tr("Empty image URL");
		QTimer::singleShot(0, this, [this]() {
			emit finished();
		});
	}
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const
{
	return m_texture;
}

QString AsyncImageResponse::errorString() const
{
	return m_error;
}

void AsyncImageResponse::replyFinished()
{
	QImage thumbnail;
	if (m_reply->error() == QNetworkReply::NoError) {
		thumbnail.loadFromData(m_reply->readAll());

		if (!thumbnail.isNull() && !m_rect.isNull() && !m_rect.isEmpty()) {
			thumbnail = thumbnail.copy(m_rect);
		}
	} else {
		m_error = m_reply->errorString();
	}
	if (thumbnail.isNull()) {
		if (m_error.isEmpty()) {
			m_error = tr("Invalid image data");
		}
	} else {
		m_texture = QQuickTextureFactory::textureFactoryForImage(thumbnail);
	}

	emit finished();
}
