#include <QtGlobal>
#if defined(Q_OS_ANDROID)

#include "android.h"
#include <QStringList>
#include <QtCore/private/qandroidextras_p.h>


bool checkPermission(const QString &perm)
{
	auto already = QtAndroidPrivate::checkPermission(perm).result();
	if (already == QtAndroidPrivate::PermissionResult::Denied) {
		auto result = QtAndroidPrivate::requestPermission(perm).result();
		if (result == QtAndroidPrivate::PermissionResult::Denied) {
			return false;
		}
	}
	return true;
}

#endif // defined(Q_OS_ANDROID)
