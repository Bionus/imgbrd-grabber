#include "models/qml-image.h"
#include <QSettings>
#include "models/profile.h"


QString QmlImage::smartPreviewUrl(int width, int height) const
{
	if (m_profile->getSettings()->value("thumbnailSmartSize", true).toBool()) {
		return m_image->mediaForSize(QSize(width, height), true).url.toString();
	} else {
		return previewUrl();
	}
}
