#ifndef SHARE_UTILS_H
#define SHARE_UTILS_H

#include <QQuickItem>


class BaseShareUtils;

class ShareUtils : public QQuickItem
{
	Q_OBJECT

	public:
		explicit ShareUtils(QQuickItem *parent = 0);

	public slots:
		void share(const QString &text);
		void sendFile(const QString &path, const QString &mimeType, const QString &title);

	private:
		BaseShareUtils *m_shareUtils;
};

#endif //SHARE_UTILS_H
