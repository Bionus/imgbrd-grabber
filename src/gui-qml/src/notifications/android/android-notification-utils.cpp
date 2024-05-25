#include "android-notification-utils.h"
#include <QJniObject>
#include "logger.h"


AndroidNotificationUtils::AndroidNotificationUtils(QQuickItem *parent)
	: BaseNotificationUtils(parent)
{}


bool AndroidNotificationUtils::notify(const QString &title, const QString &text)
{
	if (!QJniObject::isClassAvailable("org/bionus/grabber/NotificationUtils")) {
		log("Java class 'org/bionus/grabber/NotificationUtils' not available", Logger::Error);
		return false;
	}

	QJniObject javaTitle = QJniObject::fromString(title);
	QJniObject javaText = QJniObject::fromString(text);

	jboolean ok = QJniObject::callStaticMethod<jboolean>(
		"org/bionus/grabber/NotificationUtils",
		"notify",
		"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V",
		QNativeInterface::QAndroidApplication::context(),
		javaTitle.object<jstring>(),
		javaText.object<jstring>()
	);

	return (bool) ok;
}
