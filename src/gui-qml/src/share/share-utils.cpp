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

void ShareUtils::share(const QString &text, const QUrl &url)
{
	log(QString("Shared text '%1' with url '%2'").arg(text, url.toString()), Logger::Info);
	bool ok = m_shareUtils->share(text, url);
	if (!ok) {
		log("Error during sharing", Logger::Error);
	}
}
