#ifndef NETWORK_REPLY_H
#define NETWORK_REPLY_H

#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>
#include <QVariant>


class CustomNetworkAccessManager;

class NetworkReply : public QObject
{
	Q_OBJECT

	public:
		typedef QNetworkReply::NetworkError NetworkError;

		NetworkReply(QNetworkRequest request, CustomNetworkAccessManager *manager, QObject *parent = nullptr);
		NetworkReply(QNetworkRequest request, QByteArray data, CustomNetworkAccessManager *manager, QObject *parent = nullptr);
		QUrl url() const;
		QVariant attribute(QNetworkRequest::Attribute code) const;
		QByteArray readAll();
		qint64 bytesAvailable() const;
		NetworkError error() const;
		QString errorString() const;
		QNetworkReply *networkReply() const;
		bool isRunning() const;

	public slots:
		void start();
		void abort();

	signals:
		void readyRead();
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void finished();

	private:
		QNetworkRequest m_request;
		QByteArray m_data;
		CustomNetworkAccessManager *m_manager;
		bool m_post = false;
		bool m_started = false;
		QNetworkReply *m_reply = nullptr;
};

#endif // NETWORK_REPLY_H
