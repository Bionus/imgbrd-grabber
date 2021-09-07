#include "async-image-provider.h"
#include <QNetworkAccessManager>
#include <QString>
#include "async-image-response.h"
#include "models/profile.h"
#include "models/site.h"


AsyncImageProvider::AsyncImageProvider(Profile *profile)
	: m_profile(profile)
{}

QQuickImageResponse *AsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
	const int index = id.indexOf("¤");
	const QString siteKey = id.left(index);
	const QString url = id.mid(index + 1);

	Site *site = m_profile->getSites().value(siteKey);
	const auto request = site->makeRequest(site->fixUrl(url), QUrl(), "preview", nullptr, {}, false);

	auto *manager = new QNetworkAccessManager(nullptr);
	auto *reply = manager->get(request);

	auto *ret = new AsyncImageResponse(reply);
	QObject::connect(ret, &AsyncImageResponse::finished, manager, &QNetworkAccessManager::deleteLater);

	return ret;
}
