#ifndef JAVASCRIPT_CONSOLE_HELPER_H
#define JAVASCRIPT_CONSOLE_HELPER_H

#include <QObject>


class JavascriptConsoleHelper : public QObject
{
	Q_OBJECT

	public:
		explicit JavascriptConsoleHelper(const QString &prefix, QObject *parent = Q_NULLPTR);

		Q_INVOKABLE void debug(const QString &msg);
		Q_INVOKABLE void error(const QString &msg);
		Q_INVOKABLE void info(const QString &msg);
		Q_INVOKABLE void log(const QString &msg);
		Q_INVOKABLE void warn(const QString &msg);

	private:
		QString m_prefix;
};

#endif // JAVASCRIPT_CONSOLE_HELPER_H
