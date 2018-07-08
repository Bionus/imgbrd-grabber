#ifndef JAVASCRIPT_CONSOLE_HELPER_H
#define JAVASCRIPT_CONSOLE_HELPER_H

#include <QObject>


class JavascriptConsoleHelper : public QObject
{
	Q_OBJECT

	public:
		explicit JavascriptConsoleHelper(const QString &prefix, QObject *parent = Q_NULLPTR);

		Q_INVOKABLE void debug(const QString &msg) const;
		Q_INVOKABLE void error(const QString &msg) const;
		Q_INVOKABLE void info(const QString &msg) const;
		Q_INVOKABLE void log(const QString &msg) const;
		Q_INVOKABLE void warn(const QString &msg) const;

	private:
		QString m_prefix;
};

#endif // JAVASCRIPT_CONSOLE_HELPER_H
