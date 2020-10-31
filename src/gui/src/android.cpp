#include <QtGlobal>
#if defined(Q_OS_ANDROID)

#include "android.h"
#include <QStringList>
#include <QtAndroid>


bool checkPermission(const QString &perm)
{
	auto already = QtAndroid::checkPermission(perm);
	if (already == QtAndroid::PermissionResult::Denied) {
		auto results = QtAndroid::requestPermissionsSync(QStringList() << perm);
		if (results[perm] == QtAndroid::PermissionResult::Denied) {
			return false;
		}
	}
	return true;
}

#endif // defined(Q_OS_ANDROID)
