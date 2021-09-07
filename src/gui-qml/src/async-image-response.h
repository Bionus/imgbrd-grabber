#ifndef ASYNC_IMAGE_RESPONSE_H
#define ASYNC_IMAGE_RESPONSE_H

#include <QQuickImageResponse>


class QNetworkReply;
class QQuickTextureFactory;

class AsyncImageResponse : public QQuickImageResponse
{
	public:
		explicit AsyncImageResponse(QNetworkReply *reply);
		QQuickTextureFactory *textureFactory() const override;

	protected slots:
		void replyFinished();

	private:
		QNetworkReply *m_reply;
		QQuickTextureFactory *m_texture = nullptr;
};

#endif // ASYNC_IMAGE_RESPONSE_H
