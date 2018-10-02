#include "auth/auth.h"


Auth::Auth(QString type)
    : m_type(std::move(type))
{}


QString Auth::type() const
{
    return m_type;
}
