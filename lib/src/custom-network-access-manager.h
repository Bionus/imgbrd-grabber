#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QQueue>
#include <QString>


class CustomNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

	public:
		explicit CustomNetworkAccessManager(QObject *parent = nullptr);
		QNetworkReply *get(const QNetworkRequest &request);
		void sslErrorHandler(QNetworkReply *reply, const QList<QSslError> &errors);

		static QQueue<QString> NextFiles;
};

#endif // CUSTOMNETWORKACCESSMANAGER_H
