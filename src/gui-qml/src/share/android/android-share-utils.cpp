#include "android-share-utils.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include "logger.h"


AndroidShareUtils::AndroidShareUtils(QQuickItem* parent)
    : BaseShareUtils(parent)
{}

bool AndroidShareUtils::share(const QString &text, const QUrl &url)
{
	if (!QAndroidJniObject::isClassAvailable("com/bionus/grabber/ShareUtils")) {
		log("Java class 'com/bionus/grabber/ShareUtils' not available", Logger::Error);
		return false;
	}

	QAndroidJniObject jsText = QAndroidJniObject::fromString(text);
	QAndroidJniObject jsUrl = QAndroidJniObject::fromString(url.toString());

	jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>(
		"com/bionus/grabber/ShareUtils",
		"share",
		"(Ljava/lang/String;Ljava/lang/String;)Z",
		jsText.object<jstring>(),
		jsUrl.object<jstring>()
	);

	return (bool) ok;
}
