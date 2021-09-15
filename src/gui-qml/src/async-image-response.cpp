#include "async-image-response.h"
#include <QImage>
#include <QNetworkReply>
#include <QQuickTextureFactory>


AsyncImageResponse::AsyncImageResponse(QNetworkReply *reply, const QRect &rect)
	: m_reply(reply), m_rect(rect)
{
	connect(m_reply, &QNetworkReply::finished, this, &AsyncImageResponse::replyFinished);
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const
{
	return m_texture;
}

void AsyncImageResponse::replyFinished()
{
	if (m_reply->error() == QNetworkReply::NoError) {
		QImage thumbnail;
		thumbnail.loadFromData(m_reply->readAll());

		if (!m_rect.isNull() && !m_rect.isEmpty()) {
			thumbnail = thumbnail.copy(m_rect);
		}

		m_texture = QQuickTextureFactory::textureFactoryForImage(thumbnail);
	}

	emit finished();
};
