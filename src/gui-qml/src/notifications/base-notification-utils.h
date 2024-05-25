#ifndef BASE_NOTIFICATION_UTILS_H
#define BASE_NOTIFICATION_UTILS_H

#include <QQuickItem>
#include <QString>


class BaseNotificationUtils : public QQuickItem
{
	public:
		explicit BaseNotificationUtils(QQuickItem *parent = nullptr) : QQuickItem(parent){}
		virtual ~BaseNotificationUtils() {}

		virtual bool notify(const QString &title, const QString &text) { return true; }
};

#endif // BASE_NOTIFICATION_UTILS_H
