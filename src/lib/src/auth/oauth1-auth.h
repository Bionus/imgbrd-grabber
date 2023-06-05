#ifndef OAUTH1_AUTH_H
#define OAUTH1_AUTH_H

#include <QString>
#include "auth/auth.h"


class QJSValue;

class OAuth1Auth : public Auth
{
	public:
		OAuth1Auth(QString type, QString temporaryCredentialsUrl, QString authorizationUrl, QString tokenCredentialsUrl, QString signatureMethod);
		OAuth1Auth(QString type, const QJSValue &value);

		QString temporaryCredentialsUrl() const;
		QString authorizationUrl() const;
		QString tokenCredentialsUrl() const;
		QString signatureMethod() const;

		QList<AuthSettingField> settingFields() const override;

	private:
		QString m_temporaryCredentialsUrl;
		QString m_authorizationUrl;
		QString m_tokenCredentialsUrl;
		QString m_signatureMethod;
};

#endif // OAUTH1_AUTH_H
