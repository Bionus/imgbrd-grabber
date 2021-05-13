#include "loader.h"
#include <utility>


Loader::Loader(QObject *parent)
	: QObject(parent), m_status(Status::Null)
{}


Loader::Status Loader::status() const
{
	return m_status;
}

void Loader::setStatus(Status status)
{
	m_status = status;
	emit statusChanged();
}


QString Loader::error() const
{
	return m_error;
}

void Loader::setError(QString error)
{
	m_error = std::move(error);
	emit errorChanged();

	if (!error.isEmpty()) {
		setStatus(Status::Error);
	}
}
