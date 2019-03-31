#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QVariant>
#include <functional>


class Token
{
	public:
		Token() = default;
		explicit Token(const QVariant &value, const QVariant &def = QVariant());
		explicit Token(QVariant value, QString whatToDoDefault, QString emptyDefault, QString multipleDefault);
		explicit Token(std::function<QVariant()> func, bool cacheResult = true);

		QVariant value() const;
		template <typename T> T value() const { return m_value.value<T>(); }
		QString toString() const;

		const QString &whatToDoDefault() const;
		const QString &emptyDefault() const;
		const QString &multipleDefault() const;

	private:
		mutable QVariant m_value;
		QString m_whatToDoDefault;
		QString m_emptyDefault;
		QString m_multipleDefault;
		std::function<QVariant()> m_func = nullptr;
		bool m_cacheResult = false;
};

bool operator==(const Token &lhs, const Token &rhs);
bool operator!=(const Token &lhs, const Token &rhs);

Q_DECLARE_METATYPE(Token)

#endif // TOKEN_H
