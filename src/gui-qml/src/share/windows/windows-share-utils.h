#ifndef WINDOWS_SHARE_UTILS_H
#define WINDOWS_SHARE_UTILS_H

#include "share/base-share-utils.h"
#include <QUrl>


class WindowsShareUtils : public BaseShareUtils
{
	Q_OBJECT

	public:
		WindowsShareUtils(QQuickItem *parent = nullptr);

		bool share(const QString &text) override;
		bool shareUrl(const QUrl &url);
		bool sendFile(const QString &path, const QString &mimeType, const QString &title) override;
};

#endif // WINDOWS_SHARE_UTILS_H
