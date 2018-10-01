#ifndef LOGIN_H
#define LOGIN_H

#include <QObject>
#include <QString>


class QNetworkRequest;

class Login : public QObject
{
	Q_OBJECT

	public:
		enum Result
		{
			Success = 0,
			Failure = 1
		};

		virtual bool isTestable() const = 0;
		virtual QString complementUrl(QString url) const;
		virtual void complementRequest(QNetworkRequest *request) const;

	public slots:
		virtual void login() = 0;

	signals:
		void loggedIn(Login::Result result);
};

Q_DECLARE_METATYPE(Login::Result)

#endif // LOGIN_H
