#ifndef BASE_SHARE_UTILS_H
#define BASE_SHARE_UTILS_H

#include <QQuickItem>
#include <QString>


class BaseShareUtils : public QQuickItem
{
	public:
		BaseShareUtils(QQuickItem *parent = 0) : QQuickItem(parent){}
		virtual ~BaseShareUtils() {}

		virtual bool share(const QString &text) { return true; }
		virtual bool sendFile(const QString &path, const QString &mimeType, const QString &title) { return true; }
};

#endif //BASE_SHARE_UTILS_H
