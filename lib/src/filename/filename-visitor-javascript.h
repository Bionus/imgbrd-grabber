#ifndef FILENAME_VISITOR_JAVASCRIPT_H
#define FILENAME_VISITOR_JAVASCRIPT_H

#include <QJSEngine>
#include <QJSValue>
#include <QMap>
#include <QString>
#include "filename/ast/filename-visitor-base.h"


class Profile;
class QSettings;
class Token;

class FilenameVisitorJavaScript : public FilenameVisitorBase
{
	public:
		explicit FilenameVisitorJavaScript(QSettings *settings);

	protected:
		void setJavaScriptVariables(QJSEngine &engine, const QMap<QString, Token> &tokens, QJSValue obj) const;
		QString separator(const QString &key, const QString &override) const;

	private:
		QSettings *m_settings;
};

#endif // FILENAME_VISITOR_JAVASCRIPT_H
