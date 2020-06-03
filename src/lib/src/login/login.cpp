#include "login/login.h"


QString Login::complementUrl(QString url) const
{
	return url;
}

void Login::complementRequest(QNetworkRequest *request) const
{
	Q_UNUSED(request);
}
