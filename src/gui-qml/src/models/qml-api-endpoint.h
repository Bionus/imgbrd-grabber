#ifndef QML_API_ENDPOINT_H
#define QML_API_ENDPOINT_H

#include <QObject>
#include "models/api/api-endpoint.h"


class QmlApiEndpoint : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString name READ name CONSTANT)

	public:
		explicit QmlApiEndpoint(QString id, QString name, QObject *parent = nullptr)
			: QObject(parent), m_id(std::move(id)), m_name(std::move(name))
		{}

		QString id() const { return m_id; }
		QString name() const { return m_name; }

	private:
		QString m_id;
		QString m_name;
};

#endif // QML_API_ENDPOINT_H
