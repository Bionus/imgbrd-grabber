#ifndef ASYNC_IMAGE_PROVIDER_H
#define ASYNC_IMAGE_PROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QSize>
#include <QString>


class Profile;

class AsyncImageProvider : public QQuickAsyncImageProvider
{
	public:
		explicit AsyncImageProvider(Profile *profile);
		QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

	private:
		Profile *m_profile;
};

#endif // ASYNC_IMAGE_PROVIDER_H
