#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QQueue>
#include <QString>


class CustomNetworkAccessManager : public QNetworkAccessManager
{
	public:
		explicit CustomNetworkAccessManager(QObject *parent = Q_NULLPTR);
		QNetworkReply *get(const QNetworkRequest &request);
		void sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errors);

		static bool TestMode;
		static QQueue<QString> NextFiles;
};

#endif // CUSTOMNETWORKACCESSMANAGER_H
