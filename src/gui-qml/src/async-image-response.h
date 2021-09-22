#ifndef ASYNC_IMAGE_RESPONSE_H
#define ASYNC_IMAGE_RESPONSE_H

#include <QQuickImageResponse>
#include <QRect>


class QNetworkReply;
class QQuickTextureFactory;

class AsyncImageResponse : public QQuickImageResponse
{
	public:
		explicit AsyncImageResponse(QNetworkReply *reply, const QRect &rect);
		QQuickTextureFactory *textureFactory() const override;

	protected slots:
		void replyFinished();

	private:
		QNetworkReply *m_reply;
		QRect m_rect;
		QQuickTextureFactory *m_texture = nullptr;
};

#endif // ASYNC_IMAGE_RESPONSE_H
