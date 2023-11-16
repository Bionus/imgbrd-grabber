#ifndef ANDROID_SHARE_UTILS_H
#define ANDROID_SHARE_UTILS_H

#include "share/base-share-utils.h"


class AndroidShareUtils : public BaseShareUtils
{
	Q_OBJECT

	public:
		explicit AndroidShareUtils(QQuickItem *parent = nullptr);

		bool share(const QString &text) override;
		bool sendFile(const QString &path, const QString &mimeType, const QString &title) override;
};

#endif // ANDROID_SHARE_UTILS_H
