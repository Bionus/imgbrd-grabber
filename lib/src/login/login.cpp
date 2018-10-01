#include "login/login.h"


QString Login::complementUrl(QString url, QList<AuthField*> fields) const
{
	Q_UNUSED(fields);

	return url;
}

void Login::complementRequest(QNetworkRequest *request) const
{
	Q_UNUSED(request);
}
