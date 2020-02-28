#include "filename/conditional-filename.h"
#include <QSettings>
#include <utility>
#include "filename/filename-cache.h"
#include "filename/filename-condition-visitor.h"
#include "filename/filename-parser.h"
#include "logger.h"


ConditionalFilename::ConditionalFilename(QString condition, const QString &filename, QString path)
	: condition(std::move(condition)), filename(filename), path(std::move(path))
{
	if (!this->condition.isEmpty()) {
		FilenameParser parser(this->condition);
		FilenameNodeCondition *ast = parser.parseCondition();
		if (!parser.error().isEmpty()) {
			log(QString("Error parsing condition '%1': %2").arg(this->condition, parser.error()), Logger::Error);
			return;
		}

		m_ast = ast;
	}
}

bool ConditionalFilename::matches(const QMap<QString, Token> &tokens, QSettings *settings) const
{
	if (m_ast == nullptr) {
		return false;
	}

	FilenameConditionVisitor conditionVisitor(tokens, settings);
	return conditionVisitor.run(*m_ast);
}
