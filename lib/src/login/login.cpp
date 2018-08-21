#include "login/login.h"


QString Login::complementUrl(QString url, const QString &loginPart) const
{
	Q_UNUSED(loginPart);

	return url;
}

void Login::complementRequest(QNetworkRequest *request) const
{
	Q_UNUSED(request);
}
