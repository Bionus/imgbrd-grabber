#ifndef JAVASCRIPT_CONSOLE_HELPER_H
#define JAVASCRIPT_CONSOLE_HELPER_H

#include <QObject>
#include <QString>


class JavascriptConsoleHelper : public QObject
{
	Q_OBJECT

	public:
		explicit JavascriptConsoleHelper(QString prefix, QObject *parent = nullptr);

		Q_INVOKABLE void debug(const QString &msg) const;
		Q_INVOKABLE void error(const QString &msg) const;
		Q_INVOKABLE void info(const QString &msg) const;
		Q_INVOKABLE void log(const QString &msg) const;
		Q_INVOKABLE void warn(const QString &msg) const;

	private:
		QString m_prefix;
};

#endif // JAVASCRIPT_CONSOLE_HELPER_H
