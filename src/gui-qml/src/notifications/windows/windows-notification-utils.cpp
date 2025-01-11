#include "windows-notification-utils.h"
#include "vendor/windows/wintoast/wintoastlib.h"
#include "logger.h"

using namespace WinToastLib;


WindowsNotificationUtils::WindowsNotificationUtils(QQuickItem *parent)
	: BaseNotificationUtils(parent)
{
	// Ensure the current Windows version is compatible with WinToast
	if (!WinToast::isCompatible()) {
		log(QStringLiteral("Notifications unsupported on this system"), Logger::Error);
		return;
	}

	// Setup application information
	WinToast::instance()->setAppName(qApp->applicationName().toStdWString());
	WinToast::instance()->setAppUserModelId(WinToast::configureAUMI(
		qApp->organizationName().toStdWString(),
		qApp->applicationName().toStdWString(),
		L"Notifications",
		qApp->applicationVersion().toStdWString()
	));

	// Try to initialize WinToast
	WinToast::WinToastError error;
	if (!WinToast::instance()->initialize(&error)) {
		log(QStringLiteral("Could not initialize the notifications library (error %1)").arg(error), Logger::Error);
		return;
	}

	// Create a shared handler for notifications
	m_handler = new WinToastHandler();
}


bool WindowsNotificationUtils::notify(const QString &title, const QString &text)
{
	WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
	templ.setTextField(title.toStdWString(), WinToastTemplate::FirstLine);
	templ.setTextField(text.toStdWString(), WinToastTemplate::SecondLine);

	WinToast::WinToastError error;
	const auto toast_id = WinToast::instance()->showToast(templ, m_handler, &error);
	if (toast_id < 0) {
		log(QStringLiteral("Could not launch toast notification (error %1)").arg(error), Logger::Error);
		return false;
	}

	return true;
}
