#ifndef FILENAME_EXECUTION_VISITOR_H
#define FILENAME_EXECUTION_VISITOR_H

#include <QMap>
#include <QString>
#include "filename/filename-visitor-javascript.h"


class QDateTime;
class QSettings;
class QStringList;
class QVariant;
class Token;

class FilenameExecutionVisitor : public FilenameVisitorJavaScript
{
	public:
		explicit FilenameExecutionVisitor(const QMap<QString, Token> &tokens, QSettings *settings);
		void setEscapeMethod(QString (*)(const QVariant &));
		void setKeepInvalidTokens(bool keepInvalidTokens);

		QString run(const FilenameNodeRoot &node);

		void visit(const FilenameNodeConditional &node) override;
		void visit(const FilenameNodeConditionIgnore &node) override;
		void visit(const FilenameNodeConditionTag &node) override;
		void visit(const FilenameNodeConditionToken &node) override;
		void visit(const FilenameNodeJavaScript &node) override;
		void visit(const FilenameNodeText &node) override;
		void visit(const FilenameNodeVariable &node) override;

		QString variableToString(const QString &name, const QDateTime &val, const QMap<QString, QString> &options);
		QString variableToString(const QString &name, int val, const QMap<QString, QString> &options);
		QString variableToString(const QString &name, QStringList val, const QMap<QString, QString> &options);

	protected:
		void visitVariable(const QString &name, const QMap<QString, QString> &options = {});
		QString cleanVariable(QString val, const QMap<QString, QString> &options = {}) const;

	private:
		const QMap<QString, Token> &m_tokens;
		QSettings *m_settings;
		QString (*m_escapeMethod)(const QVariant &) = nullptr;
		bool m_keepInvalidTokens = false;

		QString m_result;
};

#endif // FILENAME_EXECUTION_VISITOR_H
