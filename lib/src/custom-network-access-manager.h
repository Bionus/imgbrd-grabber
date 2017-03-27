#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>


class CustomNetworkAccessManager : public QNetworkAccessManager
{
	public:
		CustomNetworkAccessManager(QObject *parent = Q_NULLPTR);
		QNetworkReply *get(const QNetworkRequest &request);
		void sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errors);

		static bool TestMode;
};

#endif // CUSTOMNETWORKACCESSMANAGER_H
