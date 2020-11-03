#ifndef HTTP_BASIC_AUTH_H
#define HTTP_BASIC_AUTH_H

#include <QString>
#include "auth/auth.h"


class HttpBasicAuth : public Auth
{
	public:
		HttpBasicAuth(QString type, int maxPage, QString passwordType);
		int maxPage() const;
		QString passwordType() const;
		QList<AuthSettingField> settingFields() const override;

	private:
		int m_maxPage;
		QString m_passwordType;
};

#endif // HTTP_BASIC_AUTH_H
