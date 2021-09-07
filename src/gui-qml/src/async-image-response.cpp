#include "async-image-response.h"
#include <QImage>
#include <QNetworkReply>
#include <QQuickTextureFactory>


AsyncImageResponse::AsyncImageResponse(QNetworkReply *reply)
	: m_reply(reply)
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
		m_texture = QQuickTextureFactory::textureFactoryForImage(thumbnail);
	}

	emit finished();
};
