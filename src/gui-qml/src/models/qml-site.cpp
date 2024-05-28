#include "models/qml-site.h"


QString QmlSite::icon() const
{
	const QString &icon = m_source->getPath().readPath("icon.png");
	#if not defined(Q_OS_ANDROID)
		return "file:///" + icon;
	#else
		return icon;
	#endif
}
