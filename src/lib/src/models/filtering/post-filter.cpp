#include "post-filter.h"
#include <QRegularExpression>
#include <QStringList>
#include "filename/ast/filename-node-condition.h"
#include "filename/filename-parser.h"
#include "filename/visitors/filename-condition-visitor.h"
#include "filter.h"
#include "filter-factory.h"
#include "logger.h"


PostFilter::PostFilter(const QString &filters)
{
	if (filters.isEmpty()) {
		return;
	}

	// Use advanced mode if the filter contains a parenthesis or conditional operator
	if (filters.contains(QRegularExpression("[()&|]"))) {
		FilenameParser parser(filters);
		FilenameNodeCondition *ast = parser.parseCondition();
		if (!parser.error().isEmpty()) {
			log(QString("Error parsing post-filter '%1': %2").arg(filters, parser.error()), Logger::Error);
			return;
		}
		m_ast = ast;
	} else {
		for (const QString &filter : filters.split(' ', Qt::SkipEmptyParts)) {
			auto fil = QSharedPointer<Filter>(FilterFactory::build(filter));
			if (!fil.isNull()) {
				m_filters.append(fil);
			}
		}
	}
}

PostFilter::PostFilter(const QStringList &filters)
	: PostFilter(filters.join(' '))
{}


QStringList PostFilter::match(const QMap<QString, Token> &tokens) const
{
	QStringList ret;

	// Advanced mode
	if (m_ast != nullptr) {
		FilenameConditionVisitor conditionVisitor(tokens, nullptr); // TODO(Bionus): properly pass settings
		if (!conditionVisitor.run(*m_ast)) {
			ret.append(QStringLiteral("post-filter"));
		}
	}

	// Simple mode
	for (const auto &filter : m_filters) {
		QString err = filter->match(tokens);
		if (!err.isEmpty()) {
			ret.append(err);
		}
	}

	return ret;
}
