#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>


class CustomNetworkAccessManager : public QNetworkAccessManager
{
	public:
		CustomNetworkAccessManager(QObject *parent = Q_NULLPTR);
		QNetworkReply *get(const QNetworkRequest &request);

		static bool TestMode;
};

#endif // CUSTOMNETWORKACCESSMANAGER_H
