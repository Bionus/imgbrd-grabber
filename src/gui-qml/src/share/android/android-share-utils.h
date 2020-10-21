#ifndef ANDROID_SHARE_UTILS_H
#define ANDROID_SHARE_UTILS_H

#include "share/base-share-utils.h"


class AndroidShareUtils : public BaseShareUtils
{
    Q_OBJECT

    public:
        AndroidShareUtils(QQuickItem* parent = 0);
        void share(const QString &text, const QUrl &url) override;
};

#endif // ANDROID_SHARE_UTILS_H
