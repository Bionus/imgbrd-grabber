#import "ios-notification-utils.h"


IosNotificationUtils::IosNotificationUtils(QQuickItem *parent)
	: BaseNotificationUtils(parent)
{}

bool IosNotificationUtils::notification(const QString &text)
{
	return true;
}
