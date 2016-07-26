#ifndef QCUSTOMNETWORKREPLY_H
#define QCUSTOMNETWORKREPLY_H

#include <QNetworkReply>

class QCustomNetworkReply : public QNetworkReply
{
    Q_OBJECT

public:
    QCustomNetworkReply( QObject *parent=0 );
    ~QCustomNetworkReply();

    void setHttpStatusCode( int code, const QByteArray &statusText = QByteArray() );
    void setHeader( QNetworkRequest::KnownHeaders header, const QVariant &value );
    void setContentType( const QByteArray &contentType );

    void setContent( const QString &content );
    void setContent( const QByteArray &content );

    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;

protected:
    qint64 readData(char *data, qint64 maxSize);

private:
    struct QCustomNetworkReplyPrivate *d;
};


#endif // QCUSTOMNETWORKREPLY_H
