#include "auth/url-auth.h"


UrlAuth::UrlAuth(QString type, QList<AuthField*> fields)
	: FieldAuth(std::move(type), std::move(fields))
{}
