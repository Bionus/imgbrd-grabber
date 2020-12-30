#include "share-utils.h"
#if defined(Q_OS_IOS)
	#include "ios/ios-share-utils.h"
#elif defined(Q_OS_ANDROID)
	#include "android/android-share-utils.h"
#else
	#include "base-share-utils.h"
#endif
#include "logger.h"


ShareUtils::ShareUtils(QQuickItem *parent)
	: QQuickItem(parent)
{
	#if defined(Q_OS_IOS)
		m_shareUtils = new IosShareUtils(this);
	#elif defined(Q_OS_ANDROID)
		m_shareUtils = new AndroidShareUtils(this);
	#else
		m_shareUtils = new BaseShareUtils(this);
	#endif
}

void ShareUtils::share(const QString &text)
{
	log(QString("Sharing text '%1'").arg(text), Logger::Debug);
	bool ok = m_shareUtils->share(text);
	if (!ok) {
		log("Error during sharing", Logger::Error);
	}
}

void ShareUtils::sendFile(const QString &path, const QString &mimeType, const QString &title)
{
	log(QString("Sharing file '%1' of type '%2' with title '%3'").arg(path, mimeType, title), Logger::Debug);
	bool ok = m_shareUtils->sendFile(path, mimeType, title);
	if (!ok) {
		log("Error during sharing", Logger::Error);
	}
}
