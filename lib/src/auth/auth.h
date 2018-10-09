#ifndef AUTH_H
#define AUTH_H

#include <QString>


class Auth
{
	public:
		explicit Auth(QString type);
		virtual ~Auth() = default;
		QString type() const;

	private:
		QString m_type;
};

#endif // AUTH_H
