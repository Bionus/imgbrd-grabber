#ifndef LOGIN_H
#define LOGIN_H

#include <QObject>


class QNetworkRequest;
class QString;

/**
 * Class representing a way to login to a source.
 *
 * Abstract base class for other login implementations.
 */
class Login : public QObject
{
	Q_OBJECT

	public:
		/**
		 * Enum representing the success or failure of a login request.
		 */
		enum Result
		{
			Success = 0,
			Failure = 1
		};

		/**
		 * Whether this login can be tested to check if it works or not.
		 * Some login types might always be testable and others never, while some might require some setting values to be set first.
		 *
		 * @return True if testable, otherwise false.
		 */
		virtual bool isTestable() const = 0;

		/**
		 * Method called before sending a request to the server. Used to override the request's URL.
		 *
		 * @param url The URL that will be called.
		 * @return The URL to call instead. Default implementation does not change the URL.
		 */
		virtual QString complementUrl(QString url) const;

		/**
		 * Method called before sending a request to the server. Used to override things like request headers.
		 *
		 * @param request A pointer to the request that will be sent to the server.
		 */
		virtual void complementRequest(QNetworkRequest *request) const;

	public slots:
		/**
		 * Trigger the actual login.
		 */
		virtual void login() = 0;

	signals:
		/**
		 * Signal to emit once the login process has finished.
		 *
		 * @param result Whether the login succeeded or not.
		 */
		void loggedIn(Login::Result result);
};

Q_DECLARE_METATYPE(Login::Result)

#endif // LOGIN_H
