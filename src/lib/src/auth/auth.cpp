#include "auth/auth.h"
#include <utility>


Auth::Auth(QString type)
	: m_type(std::move(type))
{}


QString Auth::type() const
{
	return m_type;
}

QString Auth::name() const
{
	return m_type;
}
