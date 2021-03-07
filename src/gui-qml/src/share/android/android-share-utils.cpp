#include "android-share-utils.h"
#include <QtAndroidExtras/QAndroidJniObject>
#include "logger.h"


AndroidShareUtils::AndroidShareUtils(QQuickItem* parent)
    : BaseShareUtils(parent)
{}

bool AndroidShareUtils::share(const QString &text)
{
	if (!QAndroidJniObject::isClassAvailable("com/bionus/grabber/ShareUtils")) {
		log("Java class 'com/bionus/grabber/ShareUtils' not available", Logger::Error);
		return false;
	}

	QAndroidJniObject jsText = QAndroidJniObject::fromString(text);

	jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>(
		"com/bionus/grabber/ShareUtils",
		"share",
		"(Ljava/lang/String;)Z",
		jsText.object<jstring>()
	);

	return (bool) ok;
}

bool AndroidShareUtils::sendFile(const QString &path, const QString &mimeType, const QString &title)
{
	if (!QAndroidJniObject::isClassAvailable("com/bionus/grabber/ShareUtils")) {
		log("Java class 'com/bionus/grabber/ShareUtils' not available", Logger::Error);
		return false;
	}

	QAndroidJniObject jsPath = QAndroidJniObject::fromString(path);
	QAndroidJniObject jsMimeType = QAndroidJniObject::fromString(mimeType);
	QAndroidJniObject jsTitle = QAndroidJniObject::fromString(title);

	jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>(
		"com/bionus/grabber/ShareUtils",
		"sendFile",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
		jsPath.object<jstring>(),
		jsMimeType.object<jstring>(),
		jsTitle.object<jstring>()
	);

	return (bool) ok;
}
