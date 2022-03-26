#include "async-image-provider.h"
#include <QNetworkAccessManager>
#include <QRect>
#include <QString>
#include "async-image-response.h"
#include "functions.h"
#include "models/profile.h"
#include "models/site.h"


AsyncImageProvider::AsyncImageProvider(Profile *profile)
	: m_profile(profile)
{}

QQuickImageResponse *AsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
	Q_UNUSED(requestedSize);

	const QStringList parts = id.split("¤", Qt::SkipEmptyParts);
	const QString siteKey = parts[0];
	const QString url = parts[1];

	QRect rect;
	if (parts.size() > 2) {
		rect = stringToRect(parts[2]);
	}

	Site *site = m_profile->getSites().value(siteKey);
	const auto request = site->makeRequest(site->fixUrl(url), QUrl(), "preview", nullptr, {}, false);

	auto *manager = new QNetworkAccessManager(nullptr);
	auto *reply = manager->get(request);

	auto *ret = new AsyncImageResponse(reply, rect);
	QObject::connect(ret, &AsyncImageResponse::finished, manager, &QNetworkAccessManager::deleteLater);

	return ret;
}
