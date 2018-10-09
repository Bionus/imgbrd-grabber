#ifndef OAUTH2_AUTH_H
#define OAUTH2_AUTH_H

#include <QString>
#include "auth/auth.h"


class OAuth2Auth : public Auth
{
	public:
		OAuth2Auth(QString type, QString authType, QString tokenUrl);
		QString authType() const;
		QString tokenUrl() const;

	private:
		QString m_authType;
		QString m_tokenUrl;
};

#endif // OAUTH2_AUTH_H
