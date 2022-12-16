#ifndef HTTP_BASIC_AUTH_H
#define HTTP_BASIC_AUTH_H

#include <QString>
#include "auth/auth.h"


class HttpBasicAuth : public Auth
{
	public:
		HttpBasicAuth(QString type, int maxPage, QString passwordType, QString tokenType);
		int maxPage() const;
		QString passwordType() const;
		QString tokenType() const;
		QList<AuthSettingField> settingFields() const override;

	private:
		int m_maxPage;
		QString m_passwordType;
		QString m_tokenType;
};

#endif // HTTP_BASIC_AUTH_H
