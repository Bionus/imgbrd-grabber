#include "android-share-utils.h"
#include <QtAndroidExtras/QAndroidJniObject>


AndroidShareUtils::AndroidShareUtils(QQuickItem* parent)
    : BaseShareUtils(parent)
{}

void AndroidShareUtils::share(const QString &text, const QUrl &url)
{
	QAndroidJniObject jsText = QAndroidJniObject::fromString(text);
	QAndroidJniObject jsUrl = QAndroidJniObject::fromString(url.toString());

	QAndroidJniObject::callStaticMethod<void>(
		"GrabberShareUtils",
		"share",
		"(Ljava/lang/String;Ljava/lang/String;)V",
		jsText.object<jstring>(), jsUrl.object<jstring>()
	);
}
