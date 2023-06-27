#include "chromium-user-agent.h"
#include <QCoreApplication>
#include <QOperatingSystemVersion>
#include <QString>

#if defined(Q_OS_ANDROID)
	#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		#include <QJniObject>
	#else
		#include <QAndroidJniObject>
		typedef QAndroidJniObject QJniObject;
	#endif
#endif
#if defined(Q_OS_LINUX)
	#include <sys/utsname.h>
#endif
#if defined(Q_OS_WIN)
	#include <windows.h>
#endif


bool IsTablet()
{
	return false; // TODO
}

bool IsWowX86OnAMD64()
{
	return false; // TODO
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=43;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString GetUserAgentPlatform()
{
	#if defined(Q_OS_WIN)
		return {};
	#elif defined(Q_OS_MACOS)
		return "Macintosh; ";
	#elif defined(Q_OS_LINUX)
		return "X11; ";
	#elif defined(Q_OS_ANDROID)
		return "Linux; ";
	#elif defined(Q_OS_IOS)
		return IsTablet() ? "iPad; " : "iPhone; ";
	#endif
	return {};
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=211;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString GetOSVersion()
{
	const auto osVersion = QOperatingSystemVersion::current();

	int os_major_version = osVersion.majorVersion();
	int os_minor_version = osVersion.minorVersion();
	int os_bugfix_version = osVersion.microVersion();

	#if defined(Q_OS_WIN)
		// We don't use GetVersionEx unlike Chromium here
		// See https://learn.microsoft.com/en-us/windows/win32/sysinfo/operating-system-version
		if (os_major_version == 10 || os_major_version == 11) {
			return "10.0";
		} else if (os_major_version == 8 && os_minor_version == 1) {
			return "6.3";
		} else if (os_major_version == 8) {
			return "6.2";
		} else if (os_major_version == 7) {
			return "6.1";
		}
		return {};
	#elif defined(Q_OS_MACOS)
		if (os_major_version > 10) {
			os_major_version = 10;
			os_minor_version = 15;
			os_bugfix_version = 7;
		}
		return QString("%1_%2_%3").arg(os_major_version).arg(os_minor_version).arg(os_bugfix_version);
	#elif defined(Q_OS_IOS)
		return QString("%1_%2").arg(os_major_version).arg(os_minor_version);
	#elif defined(Q_OS_ANDROID)
		const QString android_version_str = QString("%1.%2.%3%4").arg(os_major_version).arg(os_minor_version).arg(os_bugfix_version);
		const QString model = QJniObject::getStaticObjectField<jstring>("android/os/Build", "MODEL").toString();
		const QString android_info_str = model.isEmpty() ? "" : QString("; %1").arg(model);
		return QString("%1%2").arg(android_version_str).arg(android_info_str);
	#endif
	return {};
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=101;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString BuildCpuInfo()
{
	#if defined(Q_OS_MACOS)
		return "Intel";
	#elif defined(Q_OS_IOS)
		return IsTablet() ? "iPad" : "iPhone";
	#elif defined(Q_OS_WIN)

		if (IsWowX86OnAMD64()) {
			return "WOW64";
		} else {

			_SYSTEM_INFO sysinfo;
			GetNativeSystemInfo(&sysinfo);
			if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				return "Win64; x64";
			else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
				return "Win64; IA64";
		}
	#elif defined(Q_OS_LINUX)
		struct utsname unixinfo;
		uname(&unixinfo);
		if (strcmp(unixinfo.machine, "x86_64") == 0 && sizeof(void*) == sizeof(int32_t)) {
			return "i686 (x86_64)";
		} else {
			return unixinfo.machine;
		}
	#endif
	return {};
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=269;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString BuildOSCpuInfoFromOSVersionAndCpuType(const QString &os_version, const QString &cpu_type)
{
	#if defined(Q_OS_WIN)
		if (!cpu_type.isEmpty()) {
			return QString("Windows NT %1; %2").arg(os_version, cpu_type);
		} else {
			return QString("Windows NT %1").arg(os_version);
		}
	#elif defined(Q_OS_MACOS)
		return QString("%1 Mac OS X %2").arg(cpu_type, os_version);
	#elif defined(Q_OS_ANDROID)
		return QString("Android %1").arg(os_version);
	#elif defined(Q_OS_IOS)
		return QString("CPU %1 OS %2 like Mac OS X").arg(cpu_type, os_version);
	#elif defined(Q_OS_LINUX)
		struct utsname unixinfo;
		uname(&unixinfo);
		return QString("%1 %2").arg(unixinfo.sysname, cpu_type);
	#endif
	return {};
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=261;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString BuildOSCpuInfo()
{
	return BuildOSCpuInfoFromOSVersionAndCpuType(GetOSVersion(), BuildCpuInfo());
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=402;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString BuildUserAgentFromOSAndProduct(const QString& os_info, const QString& product)
{
	return QString("Mozilla/5.0 (%1) AppleWebKit/537.36 (KHTML, like Gecko) %2 Safari/537.36").arg(os_info, product);
}

// https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc;l=333;drc=31d99ff4aa0cc0b75063325ff243e911516a5a6a
QString BuildUserAgentFromProduct(const QString &product)
{
	const QString platform = GetUserAgentPlatform();
	const QString cpuInfo = BuildOSCpuInfo();
	const QString osInfo = QString("%1%2").arg(platform, cpuInfo);
	return BuildUserAgentFromOSAndProduct(osInfo, product);
}


QString buildUserAgent()
{
	const QString appName = QCoreApplication::instance()->applicationName();
	const QString appVersion = QCoreApplication::instance()->applicationVersion();
	const QString product = QString("%1/%2").arg(appName, appVersion);
	return buildUserAgentForProduct(product);
}

QString buildUserAgentForProduct(const QString &product)
{
	return BuildUserAgentFromProduct(product);
}
