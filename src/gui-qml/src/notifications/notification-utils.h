#ifndef NOTIFICATION_UTILS_H
#define NOTIFICATION_UTILS_H

#include <QQuickItem>


class BaseNotificationUtils;

class NotificationUtils : public QQuickItem
{
	Q_OBJECT

	public:
		explicit NotificationUtils(QQuickItem *parent = nullptr);

	public slots:
		void notify(const QString &title, const QString &text);

	private:
		BaseNotificationUtils *m_notificationUtils;
};

#endif // NOTIFICATION_UTILS_H
