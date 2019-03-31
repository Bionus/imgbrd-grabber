#include <QtGlobal>
#if defined(Q_OS_ANDROID)

#include "android.h"
#include <QtAndroid>


bool checkPermission(const QString &perm)
{
	auto already = QtAndroid::checkPermission(perm);
	if (already == QtAndroid::PermissionResult::Denied) {
		QtAndroid::requestPermissionsSync(QStringList() << perm);
		auto result = QtAndroid::checkPermission(perm);
		if (result == QtAndroid::PermissionResult::Denied) {
			return false;
		}
	}
	return true;
}

#endif // defined(Q_OS_ANDROID)
