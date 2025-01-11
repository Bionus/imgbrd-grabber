#ifndef WINDOWS_NOTIFICATION_UTILS_H
#define WINDOWS_NOTIFICATION_UTILS_H

#include "notifications/base-notification-utils.h"
#include <QUrl>
#include "vendor/windows/wintoast/wintoastlib.h"


class WinToastHandler : public WinToastLib::IWinToastHandler
{
	public:
		// Public interfaces
		void toastActivated() const override { /* no-op */ };
		void toastActivated(int actionIndex) const override { /* no-op */ };
		void toastDismissed(WinToastDismissalReason state) const override { /* no-op */ };
		void toastFailed() const override { /* no-op */ };
};


class WindowsNotificationUtils : public BaseNotificationUtils
{
	Q_OBJECT

	public:
		explicit WindowsNotificationUtils(QQuickItem *parent = nullptr);

		bool notify(const QString &title, const QString &text) override;

	private:
		WinToastHandler *m_handler;
};

#endif // WINDOWS_NOTIFICATION_UTILS_H
