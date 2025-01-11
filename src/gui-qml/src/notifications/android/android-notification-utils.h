#ifndef ANDROID_NOTIFICATION_UTILS_H
#define ANDROID_NOTIFICATION_UTILS_H

#include "notifications/base-notification-utils.h"


class AndroidNotificationUtils : public BaseNotificationUtils
{
	Q_OBJECT

	public:
		explicit AndroidNotificationUtils(QQuickItem *parent = nullptr);

		bool notify(const QString &title, const QString &text) override;
};

#endif // ANDROID_NOTIFICATION_UTILS_H
