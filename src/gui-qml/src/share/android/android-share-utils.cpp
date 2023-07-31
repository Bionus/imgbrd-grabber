#include "android-share-utils.h"
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

	QJniObject jsText = QJniObject::fromString(text);

	jboolean ok = QJniObject::callStaticMethod<jboolean>(
		"org/bionus/grabber/ShareUtils",
		"share",
		"(Ljava/lang/String;)Z",
		jsText.object<jstring>()
	);

	return (bool) ok;
}

bool AndroidShareUtils::sendFile(const QString &path, const QString &mimeType, const QString &title)
{
	if (!QJniObject::isClassAvailable("org/bionus/grabber/ShareUtils")) {
		log("Java class 'org/bionus/grabber/ShareUtils' not available", Logger::Error);
		return false;
	}

	QJniObject jsPath = QJniObject::fromString(path);
	QJniObject jsMimeType = QJniObject::fromString(mimeType);
	QJniObject jsTitle = QJniObject::fromString(title);

	jboolean ok = QJniObject::callStaticMethod<jboolean>(
		"org/bionus/grabber/ShareUtils",
		"sendFile",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
		jsPath.object<jstring>(),
		jsMimeType.object<jstring>(),
		jsTitle.object<jstring>()
	);

	return (bool) ok;
}
