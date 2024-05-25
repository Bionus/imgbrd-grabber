#ifndef IOS_NOTIFICATION_UTILS_H
#define IOS_NOTIFICATION_UTILS_H

#include "notifications/base-notification-utils.h"


class IosNotificationUtils : public BaseNotificationUtils
{
	Q_OBJECT

	public:
		explicit IosNotificationUtils(QQuickItem *parent = 0);

		void notify(const QString &title, const QString &text);
};

#endif // IOS_NOTIFICATION_UTILS_H
