#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QQueue>
#include <QString>


class QNetworkReply;
class QNetworkRequest;
class QSslError;

class CustomNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

	public:
		explicit CustomNetworkAccessManager(QObject *parent = nullptr);
		QNetworkReply *get(const QNetworkRequest &request);
		QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data);
		void sslErrorHandler(QNetworkReply *reply, const QList<QSslError> &errors);

		static QQueue<QString> NextFiles;

	protected:
		QNetworkReply *makeErrorReply(const QNetworkRequest &request, const QString &code = QString());
		QNetworkReply *makeTestReply(const QNetworkRequest &request);
};

#endif // CUSTOMNETWORKACCESSMANAGER_H
