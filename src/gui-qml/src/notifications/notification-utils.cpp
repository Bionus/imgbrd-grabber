#include "notification-utils.h"
#if defined(Q_OS_IOS)
	#include "ios/ios-notification-utils.h"
#elif defined(Q_OS_ANDROID)
	#include "android/android-notification-utils.h"
#elif defined(Q_OS_WIN)
	#include "windows/windows-notification-utils.h"
#else
	#include "base-notification-utils.h"
#endif
#include "logger.h"


NotificationUtils::NotificationUtils(QQuickItem *parent)
	: QQuickItem(parent)
{
	#if defined(Q_OS_IOS)
		m_notificationUtils = new IosNotificationUtils(this);
	#elif defined(Q_OS_ANDROID)
		m_notificationUtils = new AndroidNotificationUtils(this);
	#elif defined(Q_OS_WIN)
		m_notificationUtils = new WindowsNotificationUtils(this);
	#else
		m_notificationUtils = new BaseNotificationUtils(this);
	#endif
}

void NotificationUtils::notify(const QString &title, const QString &text)
{
	log(QString("Notifying '%1: %2'").arg(title, text), Logger::Debug);
	bool ok = m_notificationUtils->notify(title, text);
	if (!ok) {
		log("Error during notification", Logger::Error);
	}
}
