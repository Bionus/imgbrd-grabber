#ifndef QCUSTOMNETWORKREPLY_H
#define QCUSTOMNETWORKREPLY_H

#include <QNetworkReply>


class QCustomNetworkReply : public QNetworkReply
{
    Q_OBJECT

	public:
		explicit QCustomNetworkReply(QObject *parent = nullptr);
		~QCustomNetworkReply() override;

		void setHttpStatusCode(int code, const QByteArray &statusText = QByteArray());
		void setNetworkError(QNetworkReply::NetworkError errorCode, const QString &errorString = QString());
		void setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value);
		void setAttribute(QNetworkRequest::Attribute code, const QVariant &value);
		void setContentType(const QByteArray &contentType);

		void setContent(const QString &content);
		void setContent(const QByteArray &content);

		void abort() override;
		qint64 bytesAvailable() const override;
		bool isSequential() const override;

	protected:
		qint64 readData(char *data, qint64 maxSize) override;

	private:
		struct QCustomNetworkReplyPrivate *d;
};

#endif // QCUSTOMNETWORKREPLY_H
