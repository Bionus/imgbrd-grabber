#include "share-utils.h"
#if defined(Q_OS_IOS)
    #include "ios/ios-share-utils.h"
#elif defined(Q_OS_ANDROID)
    #include "android/android-share-utils.h"
#else
    #include "base-share-utils.h"
#endif


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
    m_shareUtils->share(text, url);
}
