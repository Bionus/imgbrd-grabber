#ifndef IOS_SHARE_UTILS_H
#define IOS_SHARE_UTILS_H

#include "share/base-share-utils.h"


class IosShareUtils : public BaseShareUtils
{
	Q_OBJECT

	public:
		explicit IosShareUtils(QQuickItem *parent = 0);

		void share(const QString &text);
		bool sendFile(const QString &path, const QString &mimeType, const QString &title) override;
};

#endif // IOS_SHARE_UTILS_H
