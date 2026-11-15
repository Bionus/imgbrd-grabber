#include "android-share-utils.h"
#include <QGuiApplication>
#include <QJniObject>
#include "logger.h"


AndroidShareUtils::AndroidShareUtils(QQuickItem* parent)
	: BaseShareUtils(parent)
{}


bool AndroidShareUtils::share(const QString &text)
{
	if (!QJniObject::isClassAvailable("org/bionus/grabber/ShareUtils")) {
		log("Java class 'org/bionus/grabber/ShareUtils' not available", Logger::Error);
		return false;
	}

	QJniObject javaText = QJniObject::fromString(text);
	jboolean ok = QJniObject::callStaticMethod<jboolean>(
		"org/bionus/grabber/ShareUtils",
		"share",
		"(Landroid/app/Activity;Ljava/lang/String;)Z",
		QNativeInterface::QAndroidApplication::context().object(),
		javaText.object<jstring>()
	);

	return (bool) ok;
}

bool AndroidShareUtils::sendFile(const QString &path, const QString &mimeType, const QString &title)
{
	if (!QJniObject::isClassAvailable("org/bionus/grabber/ShareUtils")) {
		log("Java class 'org/bionus/grabber/ShareUtils' not available", Logger::Error);
		return false;
	}

	QJniObject javaPath = QJniObject::fromString(path);
	QJniObject javaMimeType = QJniObject::fromString(mimeType);
	QJniObject javaTitle = QJniObject::fromString(title);

	jboolean ok = QJniObject::callStaticMethod<jboolean>(
		"org/bionus/grabber/ShareUtils",
		"sendFile",
		"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
		QNativeInterface::QAndroidApplication::context().object(),
		javaPath.object<jstring>(),
		javaMimeType.object<jstring>(),
		javaTitle.object<jstring>()
	);

	return (bool) ok;
}
