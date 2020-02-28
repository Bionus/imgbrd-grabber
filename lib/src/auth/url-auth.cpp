#include "auth/url-auth.h"
#include <utility>


UrlAuth::UrlAuth(QString type, QList<AuthField*> fields, int maxPage)
	: FieldAuth(std::move(type), std::move(fields)), m_maxPage(maxPage)
{}


int UrlAuth::maxPage() const
{
	return m_maxPage;
}
