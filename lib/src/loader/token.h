#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QVariant>


class Token
{
	public:
		explicit Token() = default;
		explicit Token(const QVariant &value, const QVariant &def = QVariant());
		explicit Token(const QVariant &value, const QString &whatToDoDefault, const QString &emptyDefault, const QString &multipleDefault);

		QVariant value() const;
		QString toString() const;

		QString whatToDoDefault() const;
		QString emptyDefault() const;
		QString multipleDefault() const;

	private:
		QVariant m_value;
		QString m_whatToDoDefault;
		QString m_emptyDefault;
		QString m_multipleDefault;
};

#endif // TOKEN_H
